package nuaa.aadlparser;

import java.util.List;

import org.osate.aadl2.PropertyAssociation;
import org.osate.aadl2.RangeValue;
import org.osate.aadl2.impl.IntegerLiteralImpl;
import org.osate.aadl2.impl.ModalPropertyValueImpl;
import org.osate.aadl2.impl.PropertyAssociationImpl;
import org.osate.aadl2.impl.PropertyImpl;
import org.osate.aadl2.impl.RealLiteralImpl;
import org.osate.aadl2.impl.ThreadImplementationImpl;

public class TaskInfo {
	public int[] priority(ThreadImplementationImpl[] threadImplementImpl) {
		int[] priorityArray=new int[threadImplementImpl.length];
		for(int i=0;i<threadImplementImpl.length;i++) {
		//线程属性列表   经过多步 获取线程的优先级
		List<PropertyAssociation> propertyAssociations = threadImplementImpl[i].getOwnedPropertyAssociations();
		PropertyAssociationImpl tmpPropertyAssociationImpl = (PropertyAssociationImpl) propertyAssociations.get(5);
		PropertyImpl tmpPropertyImpl = (PropertyImpl) tmpPropertyAssociationImpl.getProperty();
		ModalPropertyValueImpl modalvalueImpl = (ModalPropertyValueImpl) tmpPropertyAssociationImpl.getOwnedValues().get(0);
		IntegerLiteralImpl integervalue=(IntegerLiteralImpl)modalvalueImpl.getOwnedValue();
		int priority=(int)integervalue.getValue();
		priorityArray[i]=priority;
		}
		return priorityArray;
	}	
	
	public int[] tasksize(ThreadImplementationImpl[] threadImplementImpl) {
		int[] tasksizeArray=new int[threadImplementImpl.length];
		for(int i=0;i<threadImplementImpl.length;i++) {
		List<PropertyAssociation> propertyAssociations = threadImplementImpl[i].getOwnedPropertyAssociations();
		PropertyAssociationImpl tmpPropertyAssociationImpl = (PropertyAssociationImpl) propertyAssociations.get(2);
		PropertyImpl tmpPropertyImpl = (PropertyImpl) tmpPropertyAssociationImpl.getProperty();
		ModalPropertyValueImpl modalvalueImpl = (ModalPropertyValueImpl) tmpPropertyAssociationImpl.getOwnedValues().get(0);
		IntegerLiteralImpl integervalue=(IntegerLiteralImpl)modalvalueImpl.getOwnedValue();
		int tasksize=(int)integervalue.getValue();
		tasksizeArray[i]=tasksize;
		}
		return tasksizeArray;
	}	
	
	
	public int[] period(ThreadImplementationImpl[] threadImplementImpl) {
		int[] periodArray=new int[threadImplementImpl.length];
		for(int i=0;i<threadImplementImpl.length;i++) {
		List<PropertyAssociation> propertyAssociations = threadImplementImpl[i].getOwnedPropertyAssociations();
		PropertyAssociationImpl tmpPropertyAssociationImpl = (PropertyAssociationImpl) propertyAssociations.get(2);
		PropertyImpl tmpPropertyImpl = (PropertyImpl) tmpPropertyAssociationImpl.getProperty();
		ModalPropertyValueImpl modalvalueImpl = (ModalPropertyValueImpl) tmpPropertyAssociationImpl.getOwnedValues().get(0);
		IntegerLiteralImpl integervalue=(IntegerLiteralImpl)modalvalueImpl.getOwnedValue();
		int period=(int)integervalue.getValue();
		periodArray[i]=period;
		}
		return periodArray;
	}	
	
	public int[] execution_time(ThreadImplementationImpl[] threadImplementImpl) {
		int[] execution_timeArray=new int[threadImplementImpl.length];
		for(int i=0;i<threadImplementImpl.length;i++) {
		List<PropertyAssociation> propertyAssociations = threadImplementImpl[i].getOwnedPropertyAssociations();
		PropertyAssociationImpl tmpPropertyAssociationImpl = (PropertyAssociationImpl) propertyAssociations.get(4);
		PropertyImpl tmpPropertyImpl = (PropertyImpl) tmpPropertyAssociationImpl.getProperty();
		ModalPropertyValueImpl modalvalueImpl = (ModalPropertyValueImpl) tmpPropertyAssociationImpl.getOwnedValues().get(0);
		RealLiteralImpl max=(RealLiteralImpl)modalvalueImpl.getOwnedValue();
//		RangeValue rangevalue=(RangeValue)modalvalueImpl.getOwnedValue();
//		IntegerLiteralImpl max=(IntegerLiteralImpl)rangevalue.getMaximumValue();
		float executime1=(float)max.getValue();
		int executime=(int)executime1+1;
		execution_timeArray[i]=executime;
		}
		return execution_timeArray;
	}
	
	public double[] execution(ThreadImplementationImpl[] threadImplementImpl) {
		double[] execution_timeArray=new double[threadImplementImpl.length];
		for(int i=0;i<threadImplementImpl.length;i++) {
		List<PropertyAssociation> propertyAssociations = threadImplementImpl[i].getOwnedPropertyAssociations();
		PropertyAssociationImpl tmpPropertyAssociationImpl = (PropertyAssociationImpl) propertyAssociations.get(4);
		PropertyImpl tmpPropertyImpl = (PropertyImpl) tmpPropertyAssociationImpl.getProperty();
		ModalPropertyValueImpl modalvalueImpl = (ModalPropertyValueImpl) tmpPropertyAssociationImpl.getOwnedValues().get(0);
		RealLiteralImpl max=(RealLiteralImpl)modalvalueImpl.getOwnedValue();
//		RangeValue rangevalue=(RangeValue)modalvalueImpl.getOwnedValue();
//		IntegerLiteralImpl max=(IntegerLiteralImpl)rangevalue.getMaximumValue();
		double executime=(double)max.getValue();
		execution_timeArray[i]=executime;
		}
		return execution_timeArray;
	}
	
	public int[] deadline(ThreadImplementationImpl[] threadImplementImpl) {
		int[] deadlineArray=new int[threadImplementImpl.length];
		for(int i=0;i<threadImplementImpl.length;i++) {
		List<PropertyAssociation> propertyAssociations = threadImplementImpl[i].getOwnedPropertyAssociations();
		PropertyAssociationImpl tmpPropertyAssociationImpl = (PropertyAssociationImpl) propertyAssociations.get(1);
		PropertyImpl tmpPropertyImpl = (PropertyImpl) tmpPropertyAssociationImpl.getProperty();
		ModalPropertyValueImpl modalvalueImpl = (ModalPropertyValueImpl) tmpPropertyAssociationImpl.getOwnedValues().get(0);
		IntegerLiteralImpl integervalue=(IntegerLiteralImpl)modalvalueImpl.getOwnedValue();
		int deadline=(int)integervalue.getValue();
		deadlineArray[i]=deadline;
		}
		return deadlineArray;
	}	
}
