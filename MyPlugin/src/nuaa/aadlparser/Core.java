package nuaa.aadlparser;

import java.util.ArrayList;
import java.util.List;

import org.osate.aadl2.PropertyAssociation;
import org.osate.aadl2.impl.IntegerLiteralImpl;
import org.osate.aadl2.impl.ModalPropertyValueImpl;
import org.osate.aadl2.impl.ProcessImplementationImpl;
import org.osate.aadl2.impl.ProcessorImplementationImpl;
import org.osate.aadl2.impl.PropertyAssociationImpl;
import org.osate.aadl2.impl.PropertyImpl;
import org.osate.aadl2.impl.ThreadImplementationImpl;
import org.osate.aadl2.instance.impl.ComponentInstanceImpl;
import org.osate.aadl2.instance.impl.SystemInstanceImpl;

public class Core {
	String processorName=null;
	int coreindex=0;
	int Majar_Frame=0;

	public String[] coreArray(SystemInstanceImpl SystemInstanceImpl) {
		List<String> corearray=new ArrayList<>();
	//取到系统中处理器
	for(int i=0;i<SystemInstanceImpl.getComponentInstances().size();i++) {
		ComponentInstanceImpl Componentinstanceimpl=(ComponentInstanceImpl)SystemInstanceImpl.getComponentInstances().get(i);
		if(Componentinstanceimpl.getClassifier() instanceof ProcessorImplementationImpl) {
//			List<PropertyAssociation> propertyAssociations = Componentinstanceimpl.getOwnedPropertyAssociations();
//			PropertyAssociationImpl tmpPropertyAssociationImpl = (PropertyAssociationImpl) propertyAssociations.get(0);
//			PropertyImpl tmpPropertyImpl = (PropertyImpl) tmpPropertyAssociationImpl.getProperty();
//			ModalPropertyValueImpl modalvalueImpl = (ModalPropertyValueImpl) tmpPropertyAssociationImpl.getOwnedValues().get(0);
//			IntegerLiteralImpl integervalue=(IntegerLiteralImpl)modalvalueImpl.getOwnedValue();
//			int Majar=(int)integervalue.getValue();
//			 Majar_Frame=Majar;
			processorName=Componentinstanceimpl.getName();
			//取到处理器中各个核
			for (int j = 0; j < Componentinstanceimpl.getComponentInstances().size(); j++) {
				ComponentInstanceImpl componentinstanceimpl=(ComponentInstanceImpl)Componentinstanceimpl.getComponentInstances().get(j);
				String coreName=componentinstanceimpl.getName();
				corearray.add(coreName);
				coreindex++;
			}
		}
	}
	String[] coreArray=new String[corearray.size()];
	for(int i=0;i<coreArray.length;i++) {
		coreArray[i]=corearray.get(i);
	}
	return coreArray;
}
	
public int coreNum() {
	return coreindex;
}
public String processorName() {
	return processorName;
}
public int Majar_Frame() {
	return Majar_Frame;
}
}
