package ima2c.aadlparser;

import java.util.HashMap;
import java.util.List;
import java.util.Optional;
import java.util.OptionalLong;

import org.eclipse.emf.common.util.EList;
import org.osate.aadl2.ComponentClassifier;
import org.osate.aadl2.MemoryImplementation;
import org.osate.aadl2.MemorySubcomponent;
import org.osate.aadl2.PropertyAssociation;
import org.osate.aadl2.Subcomponent;
import org.osate.aadl2.contrib.aadlproject.SizeUnits;
import org.osate.aadl2.contrib.deployment.DeploymentProperties;
import org.osate.aadl2.contrib.deployment.MemoryProtocol;
import org.osate.aadl2.contrib.memory.MemoryProperties;
import org.osate.aadl2.impl.MemoryImplementationImpl;
import org.osate.aadl2.impl.MemorySubcomponentImpl;
import org.osate.aadl2.impl.PropertyImpl;
import org.osate.contribution.sei.arinc653.Arinc653;
import org.osate.contribution.sei.arinc653.SupportedMemoryKind;
import org.osate.pluginsupport.properties.IntegerWithUnits;

import ima2c.entity.corexml.MemoryProperty;
import ima2c.exception.ParameterException;
import ima2c.utils.ConvertorUtils;

/**
 * 
 * @author LSX
 */
@SuppressWarnings("all")
public class MemoryWalker implements AadlWalker<HashMap, MemorySubcomponent> {
	private PropertyWalker propertyWalker;

	public MemoryWalker() {
		super();
		propertyWalker = new PropertyWalker();
	}

	@Override
	public HashMap walk(MemorySubcomponent memorySubcomponent) {

		MemoryImplementationImpl memoryImpl = (MemoryImplementationImpl) memorySubcomponent
				.getComponentImplementation();
		HashMap<String, MemoryProperty> content = new HashMap<>();


		EList<Subcomponent> memorySubcomponents = memoryImpl.getAllSubcomponents();
		for (Subcomponent subcomponent : memorySubcomponents) {
			HashMap<String, MemoryProperty> subMemory = parseMemorySubcomponent(subcomponent);
			content.putAll(subMemory);
		}
		return content;
	}

	public HashMap parseMemorySubcomponent(Subcomponent subcomponent) {
		MemorySubcomponentImpl memorySubcomponentImpl = (MemorySubcomponentImpl) subcomponent;
		ComponentClassifier classifier = memorySubcomponentImpl.getClassifier();

		HashMap<String, MemoryProperty> result = new HashMap<>();
		MemoryProperty memoryProperty = new MemoryProperty();

		if (classifier instanceof MemoryImplementation) {
			MemoryImplementationImpl memoryImplementationImpl = (MemoryImplementationImpl) classifier;
			EList<PropertyAssociation> memoryProperties = memoryImplementationImpl.getOwnedPropertyAssociations();
			for (PropertyAssociation propertyAssociation : memoryProperties) {
				PropertyImpl tmpPropertyImpl = (PropertyImpl) propertyAssociation.getProperty();
				try {
					switch (tmpPropertyImpl.getName()) {
					case "Memory_Size":
						Optional<IntegerWithUnits<SizeUnits>> memorySize = MemoryProperties
								.getMemorySize(memorySubcomponentImpl);
						memoryProperty.setMemorySize(ConvertorUtils.convertSize(String.valueOf(memorySize.get())));
						break;
					case "Base_Address":
						OptionalLong baseAddress = MemoryProperties.getBaseAddress(memorySubcomponentImpl);
						memoryProperty.setPhysicalAddress(String.valueOf(baseAddress.getAsLong()));
						break;
					case "Memory_Kind":
						Optional<List<SupportedMemoryKind>> memoryKind = Arinc653.getMemoryKind(memorySubcomponentImpl);
						
						memoryProperty.setMemoryType(String.valueOf(memoryKind.get().get(0).toString())
								.replace("Memory_", "").toUpperCase());
						break;
					case "Memory_Protocol":
						Optional<MemoryProtocol> memoryProtocol = DeploymentProperties
								.getMemoryProtocol(memorySubcomponentImpl);
						
						memoryProperty.setMemoryAccess(
								memoryProtocol.orElse(MemoryProtocol.READ_ONLY).toString().toUpperCase());

						break;
					default:
						break;
					}

				} catch (Exception e) {
					System.out.println("An exception occurred in the attribute parsing of the memory component!");
					throw new ParameterException(
							"An exception occurred in the attribute parsing of the memory component!");
				}
			}
		}
		if (memoryProperty.getMemoryAccess() == null) {
			memoryProperty.setMemoryAccess("READ_WRITE");
		}
		result.put(subcomponent.getName(), memoryProperty);
		return result;
	}

}
