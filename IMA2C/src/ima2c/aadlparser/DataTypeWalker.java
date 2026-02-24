package ima2c.aadlparser;

import java.util.Optional;

import org.apache.commons.lang3.EnumUtils;
import org.osate.aadl2.contrib.aadlproject.SupportedConcurrencyControlProtocols;
import org.osate.aadl2.contrib.programming.ProgrammingProperties;
import org.osate.aadl2.contrib.thread.ThreadProperties;
import org.osate.aadl2.impl.DataAccessImpl;
import org.osate.aadl2.impl.DataSubcomponentImpl;
import org.osate.aadl2.impl.PortImpl;
import org.osate.contribution.sei.datamodel.DataModel;
import org.osate.contribution.sei.datamodel.DataRepresentation;
import org.osate.contribution.sei.datamodel.NumberRepresentation;

import ima2c.conf.SimpleDataSet;
import ima2c.utils.StringUtils;

@SuppressWarnings("all")
public class DataTypeWalker {

	public DataTypeWalker() {
		super();
	}


	public String walk4DataPort(PortImpl port, String datatype) {

		boolean validEnum = EnumUtils.isValidEnum(SimpleDataSet.class, datatype);
		if (validEnum) {
			return DataTypeWalker.dealWithType(datatype);
		}

		Optional<String> typeSourceName = ProgrammingProperties.getTypeSourceName(port);
		if (typeSourceName.orElse(null) != null) {
			return typeSourceName.get();
		}

		Optional<DataRepresentation> dataRepresentation = DataModel.getDataRepresentation(port);
		Optional<NumberRepresentation> numberRepresentation = DataModel.getNumberRepresentation(port);
		String type = dataRepresentation.get().toString();
		if (type.equalsIgnoreCase("Integer") && numberRepresentation.orElse(null) != null) {
			String header = numberRepresentation.get().toString();
			if (header.equals("Unsigned")) {
				return "unsigned " + DataTypeWalker.dealWithType(type);
			}
		}
		return DataTypeWalker.dealWithType(type);
	}


	public String walk4DataComponnet(DataSubcomponentImpl data) {

		Optional<String> typeSourceName = ProgrammingProperties.getTypeSourceName(data);
		if (typeSourceName.orElse(null) != null) {
			return typeSourceName.get();
		}

		Optional<DataRepresentation> dataRepresentation = DataModel.getDataRepresentation(data);
		Optional<NumberRepresentation> numberRepresentation = DataModel.getNumberRepresentation(data);
		String type = dataRepresentation.get().toString();
		if (type.equalsIgnoreCase("Integer") && numberRepresentation.orElse(null) != null) {
			String header = numberRepresentation.get().toString();
			if (header.equals("Unsigned")) {
				return "unsigned " + dealWithType(type);
			}
		}
		return DataTypeWalker.dealWithType(type);
	}


	public boolean isSemaphore(DataAccessImpl data) {
		boolean flag = false;
		Optional<SupportedConcurrencyControlProtocols> concurrencyControlProtocol = ThreadProperties
				.getConcurrencyControlProtocol(data);
		if (concurrencyControlProtocol.orElse(null) != null) {
			SupportedConcurrencyControlProtocols supportedConcurrencyControlProtocols = concurrencyControlProtocol
					.get();
			String value = supportedConcurrencyControlProtocols.toString();
			if (value.equals("Protected_Access") || value.equals("Semaphore")) {
				flag = true;
			}
		}
		return flag;
	}

	public static String dealWithType(String type) {
		String realtype = null;
		if (type != null) {

			switch (type.toLowerCase()) {
			case "character":
				realtype = "char";
				break;
			case "boolean":
				realtype = "bool";
				break;
			case "integer":
				realtype = "int";
				break;
			case "integer_32":
				realtype = "int";
				break;
			case "unsigned_16":
				realtype = "short";
				break;
			case "unsigned_32":
				realtype = "int";
				break;
			case "unsigned_64":
				realtype = "long";
				break;
			case "float":
				realtype = "float";
				break;
			case "double":
				realtype = "double";
				break;
			case "void":
				realtype = "void";
				break;
			case "array":

				break;
			case "enum":

				break;
			case "string":

				break;
			case "struct":

				break;
			case "union":

				break;
			default:
				return StringUtils.convert(type);
			}
		} else {
			return StringUtils.convert(type);
		}
		return realtype;
	}
}
