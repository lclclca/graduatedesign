package ima2c.codegen;

import org.stringtemplate.v4.STGroup;

public class CodeGeneratorFactory {

	public static CCodeGenerator getCCodeGenerator(STGroup group) throws Exception {
		return new CCodeGenerator(group);
	}

	public static HCodeGenerator getHCodeGenerator(String templatePath) throws Exception {
		return new HCodeGenerator(templatePath);
	}
}
