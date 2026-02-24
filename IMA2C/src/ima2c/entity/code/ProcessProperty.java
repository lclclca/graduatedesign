package ima2c.entity.code;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;

/**
 * @Description: ARINC653进程信息
 * @author LSX
 */
public class ProcessProperty {

	public ProcessProperty() {
		super();
	}

	private String threadName = null;



	private HashMap<String, ProgramCall> initializeEntrypoint = new HashMap<String, ProgramCall>();


	private String entryPoint = null;

	private List<ProgramCall> threadCalls = new LinkedList<>();

	private boolean is_Periodic = false;

	private boolean is_Aperiodic = false;

	private boolean is_Sporadic = false;

	private boolean hasSubprogram = false;

	private boolean hasQueue = false;

	private boolean hasSample = false;

	private boolean hasBlackboard = false;

	private boolean hasBuffer = false;

	private boolean hasEvent = false;

	private boolean hasSemaphore = false;

	private boolean hasCalls = false;

	/*
	 * Thread Priority
	 */
	private String basePriority = null;

	/*
	 * Thread Deadline
	 */
	private String deadline = null;

	/*
	 * Thread Period
	 */
	private String period = "-1";

	/*
	 * Thread Stack_Size
	 */
	private String stackSize = null;

	/*
	 * Thread Time_Capacity
	 */
	private String timeCapacity = null;

	/**
	 * 进程截止期类型
	 */
	private String deadlineType = "SOFT";

	/**
	 * 包含的port名
	 */
	private List<String> portNames = new LinkedList<>();

	private List<BlackBoardInfo> blackboardInfo = new LinkedList<BlackBoardInfo>();

	private List<EventInfo> eventInfo = new LinkedList<EventInfo>();

	private List<BufferInfo> bufferInfo = new LinkedList<BufferInfo>();

	private List<QueueInfo> queueInfo = new LinkedList<QueueInfo>();

	private List<SampleInfo> sampleInfo = new LinkedList<SampleInfo>();

	private List<SemaphoreInfo> semaphoreInfo = new LinkedList<SemaphoreInfo>();

	public String getThreadName() {
		return threadName;
	}

	public void setThreadName(String threadName) {
		this.threadName = threadName;
	}

	public HashMap<String, ProgramCall> getInitializeEntrypoint() {
		return initializeEntrypoint;
	}

	public void setInitializeEntrypoint(HashMap<String, ProgramCall> initializeEntrypoint) {
		this.initializeEntrypoint = initializeEntrypoint;
	}

	public List<ProgramCall> getThreadCalls() {
		return threadCalls;
	}

	public void setThreadCalls(List<ProgramCall> threadCalls) {
		this.threadCalls = threadCalls;
	}

	public String getEntryPoint() {
		return entryPoint;
	}

	public void setEntryPoint(String entryPoint) {
		this.entryPoint = entryPoint;
	}

	public boolean isIs_Periodic() {
		return is_Periodic;
	}

	public void setIs_Periodic(boolean is_Periodic) {
		this.is_Periodic = is_Periodic;
	}

	public boolean isIs_Aperiodic() {
		return is_Aperiodic;
	}

	public void setIs_Aperiodic(boolean is_Aperiodic) {
		this.is_Aperiodic = is_Aperiodic;
	}

	public boolean isIs_Sporadic() {
		return is_Sporadic;
	}

	public void setIs_Sporadic(boolean is_Sporadic) {
		this.is_Sporadic = is_Sporadic;
	}

	public boolean isHasSubprogram() {
		return hasSubprogram;
	}

	public void setHasSubprogram(boolean hasSubprogram) {
		this.hasSubprogram = hasSubprogram;
	}

	public boolean isHasQueue() {
		return hasQueue;
	}

	public void setHasQueue(boolean hasQueue) {
		this.hasQueue = hasQueue;
	}

	public boolean isHasSample() {
		return hasSample;
	}

	public void setHasSample(boolean hasSample) {
		this.hasSample = hasSample;
	}

	public boolean isHasBlackboard() {
		return hasBlackboard;
	}

	public void setHasBlackboard(boolean hasBlackboard) {
		this.hasBlackboard = hasBlackboard;
	}

	public boolean isHasBuffer() {
		return hasBuffer;
	}

	public void setHasBuffer(boolean hasBuffer) {
		this.hasBuffer = hasBuffer;
	}

	public boolean isHasEvent() {
		return hasEvent;
	}

	public void setHasEvent(boolean hasEvent) {
		this.hasEvent = hasEvent;
	}

	public boolean isHasSemaphore() {
		return hasSemaphore;
	}

	public void setHasSemaphore(boolean hasSemaphore) {
		this.hasSemaphore = hasSemaphore;
	}

	public boolean isHasCalls() {
		return hasCalls;
	}

	public void setHasCalls(boolean hasCalls) {
		this.hasCalls = hasCalls;
	}

	public String getBasePriority() {
		return basePriority;
	}

	public void setBasePriority(String basePriority) {
		this.basePriority = basePriority;
	}

	public String getDeadline() {
		return deadline;
	}

	public void setDeadline(String deadline) {
		this.deadline = deadline;
	}

	public String getPeriod() {
		return period;
	}

	public void setPeriod(String period) {
		this.period = period;
	}

	public String getStackSize() {
		return stackSize;
	}

	public void setStackSize(String stackSize) {
		this.stackSize = stackSize;
	}

	public String getTimeCapacity() {
		return timeCapacity;
	}

	public void setTimeCapacity(String timeCapacity) {
		this.timeCapacity = timeCapacity;
	}

	public List<String> getPortNames() {
		return portNames;
	}

	public void setPortNames(List<String> portNames) {
		this.portNames = portNames;
	}

	public List<BlackBoardInfo> getBlackboardInfo() {
		return blackboardInfo;
	}

	public void setBlackboardInfo(List<BlackBoardInfo> blackboardInfo) {
		this.blackboardInfo = blackboardInfo;
	}

	public List<EventInfo> getEventInfo() {
		return eventInfo;
	}

	public void setEventInfo(List<EventInfo> eventInfo) {
		this.eventInfo = eventInfo;
	}

	public List<BufferInfo> getBufferInfo() {
		return bufferInfo;
	}

	public void setBufferInfo(List<BufferInfo> bufferInfo) {
		this.bufferInfo = bufferInfo;
	}

	public List<QueueInfo> getQueueInfo() {
		return queueInfo;
	}

	public void setQueueInfo(List<QueueInfo> queueInfo) {
		this.queueInfo = queueInfo;
	}

	public List<SampleInfo> getSampleInfo() {
		return sampleInfo;
	}

	public void setSampleInfo(List<SampleInfo> sampleInfo) {
		this.sampleInfo = sampleInfo;
	}

	public List<SemaphoreInfo> getSemaphoreInfo() {
		return semaphoreInfo;
	}

	public void setSemaphoreInfo(List<SemaphoreInfo> semaphoreInfo) {
		this.semaphoreInfo = semaphoreInfo;
	}

	public String getDeadlineType() {
		return deadlineType;
	}

	public void setDeadlineType(String deadlineType) {
		this.deadlineType = deadlineType;
	}

	@Override
	public String toString() {

		System.out.println("\n\n");
		System.out.println("----------thead Information----------");
		System.out.println("threadName: " + threadName);
		System.out.println("initialize_Entrypoint: " + initializeEntrypoint);
		System.out.println("entry_point: " + entryPoint);
		System.out.println("threadCalls: " + threadCalls);
		System.out.println("base_priority: " + basePriority);
		System.out.println("deadline: " + deadline);
		System.out.println("period: " + period);
		System.out.println("stack_size: " + stackSize);
		System.out.println("time_capacity: " + timeCapacity);
		System.out.println("deadline type:" + deadlineType);

		System.out.println("\n");

		System.out.println("----------Inter-partition communication information----------");
		System.out.println(threadName + " Sampling: " + sampleInfo);
		System.out.println(threadName + " Queuing: " + queueInfo);

		System.out.println("\n");

		System.out.println("----------Intra-partition communication information----------");
		System.out.println(threadName + " BlackBoard: " + blackboardInfo);
		System.out.println(threadName + " Event: " + eventInfo);
		System.out.println(threadName + " Buffer: " + bufferInfo);
		System.out.println(threadName + " Semaphore: " + semaphoreInfo);

		System.out.println("\n\n");
		return "";
	}

}
