package ima2c.aadlparser;

import static java.util.stream.Collectors.toList;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Optional;
import java.util.OptionalLong;
import java.util.concurrent.atomic.AtomicInteger;

import org.eclipse.emf.common.util.EList;
import org.osate.aadl2.AccessConnection;
import org.osate.aadl2.Classifier;
import org.osate.aadl2.Connection;
import org.osate.aadl2.ConnectionEnd;
import org.osate.aadl2.Context;
import org.osate.aadl2.Data;
import org.osate.aadl2.PortConnection;
import org.osate.aadl2.contrib.aadlproject.TimeUnits;
import org.osate.aadl2.contrib.communication.CommunicationProperties;
import org.osate.aadl2.contrib.programming.ProgrammingProperties;
import org.osate.aadl2.impl.AccessConnectionImpl;
import org.osate.aadl2.impl.ConnectedElementImpl;
import org.osate.aadl2.impl.DataAccessImpl;
import org.osate.aadl2.impl.DataPortImpl;
import org.osate.aadl2.impl.DataSubcomponentImpl;
import org.osate.aadl2.impl.EventDataPortImpl;
import org.osate.aadl2.impl.EventPortImpl;
import org.osate.aadl2.impl.PortConnectionImpl;
import org.osate.aadl2.impl.ProcessImplementationImpl;
import org.osate.aadl2.impl.ProcessSubcomponentImpl;
import org.osate.aadl2.impl.SubprogramTypeImpl;
import org.osate.aadl2.impl.ThreadSubcomponentImpl;
import org.osate.contribution.sei.arinc653.Arinc653;
import org.osate.contribution.sei.arinc653.QueueingDisciplineType;
import org.osate.pluginsupport.properties.IntegerWithUnits;

import ima2c.conf.FeatureNameConf;
import ima2c.entity.code.BlackBoardInfo;
import ima2c.entity.code.BufferInfo;
import ima2c.entity.code.CommunicationProperty;
import ima2c.entity.code.EventInfo;
import ima2c.entity.code.ProgramCall;
import ima2c.entity.code.QueueInfo;
import ima2c.entity.code.SampleInfo;
import ima2c.entity.code.SemaphoreInfo;
import ima2c.entity.corexml.Channel;
import ima2c.entity.corexml.ConnectPort;
import ima2c.utils.ConvertorUtils;
import ima2c.utils.StringUtils;

@SuppressWarnings("all")
public class ConnectionWalker implements AadlWalker<CommunicationProperty, ProcessSubcomponentImpl> {

	private DataTypeWalker dataWalker;
	private PropertyWalker propertyWalker;
	private SubprogramWalker subprogramWalker;

	public ConnectionWalker() {
		super();
		dataWalker = new DataTypeWalker();
		propertyWalker = new PropertyWalker();
		subprogramWalker = new SubprogramWalker();
	}

	public List walk4communication(EList<Connection> connections) {
		List<Channel> result = new ArrayList<>();
		AtomicInteger index = new AtomicInteger(1);
		for (int i = 0; i < connections.size(); i++) {
			if (connections.get(i) instanceof AccessConnectionImpl) {
				continue;
			}
			Channel channel = new Channel();
			channel.setChannelId(String.valueOf(index.get()));
			index.getAndAdd(1);
			PortConnectionImpl portConnect = (PortConnectionImpl) connections.get(i);
			channel.setChannelName(portConnect.getName());

			ConnectPort sourcePort = new ConnectPort();
			ConnectedElementImpl source = (ConnectedElementImpl) portConnect.getSource();
			ProcessSubcomponentImpl sourceContext = (ProcessSubcomponentImpl) source.getContext();
			if (sourceContext == null) {
				continue;
			}
			ConnectionEnd sourceEnd = source.getConnectionEnd();

			/*
			 * ProcessImplementationImpl contextSourceType = (ProcessImplementationImpl)
			 * sourceContext .getProcessSubcomponentType();
			 */

			sourcePort.setDirection(FeatureNameConf.getSource());
			sourcePort.setPartitionName(sourceContext.getName());
			sourcePort.setPartitionId(SystemWalker.partitionMap.get(sourceContext.getName()));
			sourcePort.setPortName(sourceEnd.getName());

			channel.setSource(sourcePort);
			// destination
			ConnectPort destinationPort = new ConnectPort();
			ConnectedElementImpl destination = (ConnectedElementImpl) portConnect.getDestination();

			ProcessSubcomponentImpl destContext = (ProcessSubcomponentImpl) destination.getContext();

			if (destContext == null) {
				continue;
			}
			ConnectionEnd destEnd = destination.getConnectionEnd();

			destinationPort.setDirection(FeatureNameConf.getDestination());
			destinationPort.setPartitionName(destContext.getName());
			destinationPort.setPartitionId(SystemWalker.partitionMap.get(destContext.getName()));
			destinationPort.setPortName(destEnd.getName());

			channel.setDestination(destinationPort);
			result.add(channel);
		}

		return result;

	}

	@Override
	public CommunicationProperty walk(ProcessSubcomponentImpl process) {
		ProcessImplementationImpl processImpl = (ProcessImplementationImpl) process.getComponentImplementation();
		String processName = StringUtils.convert(process.getFullName());
		CommunicationProperty communication = new CommunicationProperty();
		EList<Connection> connections = processImpl.getAllConnections();
		for (Connection connection : connections) {
			if (connection instanceof PortConnection) {
				PortConnectionImpl portConnect = (PortConnectionImpl) connection;

				Context sourceContext = portConnect.getSource().getContext();
				Context destinationContext = portConnect.getDestination().getContext();

				ConnectionEnd sourceEnd = portConnect.getSource().getConnectionEnd();
				ConnectionEnd destinationEnd = portConnect.getDestination().getConnectionEnd();
				if (sourceEnd instanceof DataPortImpl) {
					// 分区间Sampling / 分区内Blackboard
					if (communicationType(sourceContext, destinationContext)) {
						// process <--------> thread
						// 分区间通信------>Sampling
						SampleInfo sampleInfo = new SampleInfo();
						String header = null;
						DataPortImpl dataPortImpl = null;
						if (sourceContext == null) {
							dataPortImpl = (DataPortImpl) sourceEnd;
							header = StringUtils.convert(processName + "_" + dataPortImpl.getName());
							sampleInfo.setDirection(true);
							sampleInfo.getThreadName().add(destinationContext.getName());
							sampleInfo.setValidityType(header + "_valid");
							sampleInfo.setStatus(header + "_status");
							sampleInfo.setPort_direction("DESTINATION");
						} else {
							dataPortImpl = (DataPortImpl) destinationEnd;
							// Out Port && process port在connection右侧 WRITE_SAMPLING_MESSAGE
							header = StringUtils.convert(processName + "_" + dataPortImpl.getName());
							sampleInfo.setDirection(false);
							sampleInfo.getThreadName().add(sourceContext.getName());
							sampleInfo.setPort_direction("SOURCE");
						}
						sampleInfo.setId(header + "_id");
						sampleInfo.setPortName(StringUtils.convert(dataPortImpl.getName()));
						sampleInfo.setLength(header + "_length");
						sampleInfo.setMessageAddr(header + "_dvalue");

						String dataType = dataPortImpl.getDataFeatureClassifier().getName();
						sampleInfo.setDataType(dataType.toLowerCase());
						communication.getDatatype().put(dataType.toLowerCase(),
								dataWalker.walk4DataPort(dataPortImpl, dataType));

						Optional<IntegerWithUnits<TimeUnits>> samplingRefreshPeriod = Arinc653
								.getSamplingRefreshPeriod(dataPortImpl);
						if (samplingRefreshPeriod.orElse(null) != null) {
							sampleInfo.setRefresh(
									ConvertorUtils.convertTime(String.valueOf(samplingRefreshPeriod.get())));
						}
						communication.getSampleInfo().add(sampleInfo);
						/**
						 * Check:打印采样端口信息
						 */
						// System.out.println("\n\n" + sampleInfo + "\n\n");
					} else {
						// 分区内通信------>Blackboard,左侧out port,右侧in port,取接in port的信息
						BlackBoardInfo blackBoardInfo = new BlackBoardInfo();
						String portName = StringUtils.convert(destinationEnd.getName());
						blackBoardInfo.setPortName(StringUtils.convert(portName));
						blackBoardInfo.setId(portName + "_id");
						DataPortImpl dataPortImpl = (DataPortImpl) destinationEnd;
						String dataType = dataPortImpl.getDataFeatureClassifier().getName();
						blackBoardInfo.setDataType(dataType.toLowerCase());
						communication.getDatatype().put(dataType.toLowerCase(),
								dataWalker.walk4DataPort(dataPortImpl, dataType));
						blackBoardInfo.setStatus(portName + "_status");
						blackBoardInfo.setMessageAddr(portName + "_dvalue");
						blackBoardInfo.setLength(portName + "_length");
						
						blackBoardInfo.getThreadName().add(sourceContext.getName());
						blackBoardInfo.getThreadName().add(destinationContext.getName());
						Optional<IntegerWithUnits<TimeUnits>> timeout = Arinc653.getTimeout(dataPortImpl);
						if (timeout.orElse(null) != null) {
							blackBoardInfo.setTimeOut(ConvertorUtils.convertTime(String.valueOf(timeout.get())));
						} else {
							blackBoardInfo.setTimeOut("-1");
						}
						blackBoardInfo.getPorts().add(sourceEnd.getName());
						blackBoardInfo.getPorts().add(destinationEnd.getName());
						
						communication.getBlackboardInfo().add(blackBoardInfo);

						/**
						 * Check:打印blackBoard信息
						 */
						// System.out.println("\n\n" + blackBoardInfo.toString() + "\n\n");

					}
				} else if (sourceEnd instanceof EventPortImpl) {
					// 分区内Event
					EventInfo eventInfo = new EventInfo();
					EventPortImpl eventPortImpl = (EventPortImpl) destinationEnd;
					String portName = StringUtils.convert(eventPortImpl.getName());
					eventInfo.setPortName(StringUtils.convert(portName));
					eventInfo.setId(portName + "_id");
					eventInfo.setStatus(portName + "_status");
					Optional<IntegerWithUnits<TimeUnits>> timeout = Arinc653.getTimeout(eventPortImpl);
					if (timeout.orElse(null) != null) {
						eventInfo.setTimeOut(ConvertorUtils.convertTime(String.valueOf(timeout.get())));
					} else {
						eventInfo.setTimeOut("-1");
					}

					Optional<Classifier> computeEntrypoint = ProgrammingProperties.getComputeEntrypoint(eventPortImpl);
					if ((computeEntrypoint.orElse(null) != null)) {
						SubprogramTypeImpl typeImpl = (SubprogramTypeImpl) computeEntrypoint.get();
						// TODO:这里的connection暂时先设置为null,如果Compute_Entrypoint调用的subprogram 调用的是有参数的,
						// 那么需要从process依次解析thread,再遍历所有的thread的features，查找匹配的feature对应的thread
						ProgramCall programCall = subprogramWalker.walk(null, typeImpl);
						eventInfo.getComputeEntrypoint().put(typeImpl.getName(), programCall);
					}
					eventInfo.getThreadName().add(sourceContext.getName());
					eventInfo.getThreadName().add(destinationContext.getName());

					eventInfo.getPorts().add(sourceEnd.getName());
					eventInfo.getPorts().add(destinationEnd.getName());

					// 添加到通信中
					communication.getEventInfo().add(eventInfo);
					/**
					 * Check EventInfo
					 */
					// System.out.println(eventInfo.toString());

				} else if (sourceEnd instanceof EventDataPortImpl) {
					if (communicationType(sourceContext, destinationContext)) {
						// 分区间通信------>Queuing
						QueueInfo queueInfo = new QueueInfo();
						EventDataPortImpl eventDataPortImpl = null;
						String header = null;
						if (sourceContext == null) {
							eventDataPortImpl = (EventDataPortImpl) sourceEnd;
							// In port && process port在connection左侧,RECEIVE_QUEUING_MESSAGE
							header = StringUtils.convert(processName + "_" + eventDataPortImpl.getName());
							queueInfo.setDirection(true);
							queueInfo.getThreadName().add(destinationContext.getName());
							// 接收消息需要的属性
							queueInfo.setStatus(header + "_status");
							queueInfo.setPort_direction("DESTINATION");

						} else {
							eventDataPortImpl = (EventDataPortImpl) destinationEnd;
							// Out Port && process port在connection左侧,SEND_QUEUING_MESSAGE
							header = StringUtils.convert(processName + "_" + eventDataPortImpl.getName());
							queueInfo.setDirection(false);
							queueInfo.getThreadName().add(sourceContext.getName());
							queueInfo.setPort_direction("SOURCE");
						}
						queueInfo.setId(header + "_id");
						queueInfo.setPortName(StringUtils.convert(eventDataPortImpl.getName()));
						queueInfo.setLength(header + "_length");
						queueInfo.setMessageAddr(header + "_dvalue");

						
						String dataType = eventDataPortImpl.getDataFeatureClassifier().getName();
						queueInfo.setDataType(dataType.toLowerCase());
						communication.getDatatype().put(dataType.toLowerCase(),
								dataWalker.walk4DataPort(eventDataPortImpl, dataType));
						// queuing特有的ARINC653属性
						Optional<QueueingDisciplineType> queueingDiscipline = Arinc653
								.getQueueingDiscipline(eventDataPortImpl);
						if (queueingDiscipline.orElse(null) != null) {
							if (queueingDiscipline.get().toString().equals("By_Priority")) {
								queueInfo.setQueuingDiscipline("PRIORITY");
							}
						} else {
							queueInfo.setQueuingDiscipline(String.valueOf(
									queueingDiscipline.orElse(QueueingDisciplineType.FIFO).toString().toUpperCase()));
						}
						OptionalLong queueSize = CommunicationProperties.getQueueSize(eventDataPortImpl);
						
						if ((queueSize.orElse(0L) != 0)) {
							queueInfo.setQueueSize(String.valueOf(queueSize.getAsLong()));
						}
						// 默认设置为4
//						else {
//							queueInfo.setQueueSize(String.valueOf(queueSize.orElse(4l)));
//						}
						Optional<IntegerWithUnits<TimeUnits>> timeout = Arinc653.getTimeout(eventDataPortImpl);
						if (timeout.orElse(null) != null) {
							queueInfo.setTimeOut(ConvertorUtils.convertTime(String.valueOf(timeout.get())));
						} else {
							// 沒有默认设置为0
							queueInfo.setTimeOut("-1");
						}

						// 添加到通信中
						communication.getQueueInfo().add(queueInfo);
						/**
						 * Check:打印队列端口信息
						 */
						// System.out.println("\n\n" + queueInfo.toString() + "\n\n");

					} else {
						// 分区内通信------>Buffer
						BufferInfo bufferInfo = new BufferInfo();
						EventDataPortImpl eventDataPortImpl = (EventDataPortImpl) destinationEnd;
						String portName = StringUtils.convert(eventDataPortImpl.getName());
						bufferInfo.setPortName(StringUtils.convert(portName));
						bufferInfo.setId(portName + "_id");
						bufferInfo.getThreadName().add(sourceContext.getName());
						bufferInfo.getThreadName().add(destinationContext.getName());

						bufferInfo.setLength(portName + "_length");
						bufferInfo.setMessageAddr(portName + "_dvalue");
						bufferInfo.setStatus(portName + "_status");
						Optional<IntegerWithUnits<TimeUnits>> timeout = Arinc653.getTimeout(eventDataPortImpl);
						// 默认为0
						if ((timeout.orElse(null) != null)) {
							bufferInfo.setTimeOut(ConvertorUtils.convertTime(String.valueOf(timeout.get())));
						} else {
							bufferInfo.setTimeOut("-1");
						}

						OptionalLong queueSize = CommunicationProperties.getQueueSize(eventDataPortImpl);
						// 默认设置为6
						if ((queueSize.orElse(1L) != 1)) {
							bufferInfo.setMaxNbMessage(String.valueOf(queueSize.getAsLong()));

						}
						bufferInfo.setQueuingDiscipline(
								propertyWalker.walk(eventDataPortImpl, Arinc653.QUEUEING_DISCIPLINE__NAME));

						String dataType = eventDataPortImpl.getDataFeatureClassifier().getName();
						bufferInfo.setDataType(dataType.toLowerCase());
						communication.getDatatype().put(dataType.toLowerCase(),
								dataWalker.walk4DataPort(eventDataPortImpl, dataType));

						bufferInfo.getPorts().add(sourceEnd.getName());
						bufferInfo.getPorts().add(destinationEnd.getName());
						communication.getBufferInfo().add(bufferInfo);
						/**
						 * Cheak Buffer
						 */
						// System.out.println(bufferInfo.toString());
					}
				}

			}

			/**
			 * 这里考虑data access
			 */
			else if (connection instanceof AccessConnection) {
				AccessConnectionImpl accessConnection = (AccessConnectionImpl) connection;
				Context sourceContext = accessConnection.getSource().getContext();
				Context destinationContext = accessConnection.getDestination().getContext();

				ConnectionEnd sourceEnd = accessConnection.getSource().getConnectionEnd();
				ConnectionEnd destinationEnd = accessConnection.getDestination().getConnectionEnd();

				DataAccessImpl dataAccessImpl = (DataAccessImpl) destinationEnd;
				DataSubcomponentImpl dataSub = (DataSubcomponentImpl) sourceEnd;
				Data data = (Data) dataAccessImpl.getClassifier();
				if (dataWalker.isSemaphore(dataAccessImpl)) {
					// Semaphore通信
					SemaphoreInfo semaphoreInfo = new SemaphoreInfo();
					String portName = StringUtils.convert(dataSub.getName());
					semaphoreInfo.setId(portName + "_id");
					semaphoreInfo.setPortName(StringUtils.convert(portName));
					semaphoreInfo.setDataType(data.getName().toLowerCase());

					communication.getDatatype().put(data.getName().toLowerCase(),
							dataWalker.walk4DataComponnet(dataSub));
					// TODO: semaphore的方向参照note.md
					semaphoreInfo.getThreadName().add(destinationContext.getName());
					Optional<QueueingDisciplineType> queueingDiscipline = Arinc653
							.getQueueingDiscipline(dataAccessImpl);
					if ((queueingDiscipline.orElse(null) != null)) {
						if (queueingDiscipline.get().toString().equals("By_Priority")) {
							semaphoreInfo.setQueuingDiscipline("PRIORITY");
						} else {
							semaphoreInfo.setQueuingDiscipline(queueingDiscipline.get().toString().toUpperCase());
						}
					}

					semaphoreInfo.setStatus(portName + "_status");
					Optional<IntegerWithUnits<TimeUnits>> timeout = Arinc653.getTimeout(accessConnection);
					// 默认为0
					if ((timeout.orElse(null) != null)) {
						semaphoreInfo.setTimeOut(ConvertorUtils.convertTime(String.valueOf(timeout.get())));
					} else {
						semaphoreInfo.setTimeOut("-1");
					}
					semaphoreInfo.getPorts().add(dataAccessImpl.getName());

					communication.getSemaphoreInfo().add(semaphoreInfo);

				} else {
					// Blackboard通信
					BlackBoardInfo blackBoardInfo = new BlackBoardInfo();
					String portName = StringUtils.convert(dataSub.getName());
					blackBoardInfo.setId(portName + "_id");
					blackBoardInfo.setPortName(StringUtils.convert(portName));
					blackBoardInfo.setLength(portName + "_length");
					blackBoardInfo.setStatus(portName + "_status");
					blackBoardInfo.setDirection(false);
					blackBoardInfo.setMessageAddr(portName + "_dvalue");

					blackBoardInfo.setDataType(data.getName().toLowerCase());
					blackBoardInfo.getThreadName().add(destinationContext.getName());
					communication.getDatatype().put(data.getName().toLowerCase(),
							dataWalker.walk4DataComponnet(dataSub));
					Optional<IntegerWithUnits<TimeUnits>> timeout = Arinc653.getTimeout(accessConnection);
					// 默认为0
					if ((timeout.orElse(null) != null)) {
						blackBoardInfo.setTimeOut(ConvertorUtils.convertTime(String.valueOf(timeout.get())));
					} else {
						blackBoardInfo.setTimeOut("-1");
					}
					blackBoardInfo.getPorts().add(dataAccessImpl.getName());
					communication.getBlackboardInfo().add(blackBoardInfo);
					/**
					 * Check blackBoard
					 */
					// System.out.println(blackBoardInfo.toString());
				}
			}

			/**
			 * 出现异常情况
			 */
			else {
				// System.out.println("Port parsing error, DIMA does not currently support this
				// communication method!");
			}
		}

		// blackboard & Semaphore去重
		List<SemaphoreInfo> semaphoreInfos = new LinkedList<>();
		semaphoreInfos.addAll(communication.getSemaphoreInfo());
		communication.getSemaphoreInfo().clear();
		for (int i = 0; i < semaphoreInfos.size(); i++) {
			SemaphoreInfo tmpSemaphore = semaphoreInfos.get(i);
			if (tmpSemaphore.getThreadName().size() == 0) {
				continue;
			}
			if (tmpSemaphore.getThreadName().size() > 1) {
				communication.getSemaphoreInfo().add(tmpSemaphore);
				continue;
			}
			String blackBoardName = tmpSemaphore.getPortName();

			List<SemaphoreInfo> collect = semaphoreInfos.parallelStream()
					.filter(sem -> sem.getPortName().equals(blackBoardName)).collect(toList());

			String otherThreadName = new String();
			if (collect.size() > 0) {
				for (SemaphoreInfo repeatSemaphore : collect) {
					if (repeatSemaphore.equals(tmpSemaphore)) {
						continue;
					}
					otherThreadName = repeatSemaphore.getThreadName().get(0);
					tmpSemaphore.getThreadName().add(otherThreadName);
					tmpSemaphore.getPorts().addAll(repeatSemaphore.getPorts());
					repeatSemaphore.getThreadName().clear();
				}
			}
			communication.getSemaphoreInfo().add(tmpSemaphore);
		}
		// System.out.println(communication.getSemaphoreInfo());

		List<BlackBoardInfo> blackBoardInfos = new LinkedList<>();
		blackBoardInfos.addAll(communication.getBlackboardInfo());
		communication.getBlackboardInfo().clear();
		for (int i = 0; i < blackBoardInfos.size(); i++) {
			BlackBoardInfo tmpBlackBoard = blackBoardInfos.get(i);
			if (tmpBlackBoard.getThreadName().size() == 0) {
				continue;
			}
			if (tmpBlackBoard.getThreadName().size() > 1) {
				communication.getBlackboardInfo().add(tmpBlackBoard);
				continue;
			}
			String blackBoardName = tmpBlackBoard.getPortName();

			// blackBoardInfos.remove(tmpBlackBoard);
			List<BlackBoardInfo> collect = blackBoardInfos.parallelStream()
					.filter(black -> black.getPortName().equals(blackBoardName)).collect(toList());

			String otherThreadName = new String();
			if (collect.size() > 0) {
				for (BlackBoardInfo repeatBlackBoard : collect) {
					if (repeatBlackBoard.equals(tmpBlackBoard)) {
						continue;
					}
					otherThreadName = repeatBlackBoard.getThreadName().get(0);
					tmpBlackBoard.getThreadName().add(otherThreadName);
					tmpBlackBoard.getPorts().addAll(repeatBlackBoard.getPorts());
					repeatBlackBoard.getThreadName().clear();
				}
			}
			communication.getBlackboardInfo().add(tmpBlackBoard);
		}
		// // System.out.println(communication.getBlackboardInfo());
		// communication.datatype.forEach((k, v) -> // System.out.println("aadl type:" +
		// k
		// + ",C type:" + v));
		return communication;
	}

	public boolean communicationType(Context sourceContext, Context destinationContext) {
		boolean flag = true;
		if ((sourceContext instanceof ThreadSubcomponentImpl)
				&& (destinationContext instanceof ThreadSubcomponentImpl)) {
			flag = false;
		}
		return flag;
	}

}
