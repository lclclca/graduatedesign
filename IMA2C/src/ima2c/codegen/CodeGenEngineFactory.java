package ima2c.codegen;

import java.util.HashMap;

import org.stringtemplate.v4.STGroup;

public class CodeGenEngineFactory {

	public static CCodeGenEngine getCCodeGenEngine(STGroup group, HashMap content, String filePath, String fileName,
			String parameter) throws Exception {
		return new CCodeGenEngine(CodeGeneratorFactory.getCCodeGenerator(group), content, fileName, filePath,
				parameter);
	}

	public static HCodeGenEngine getHCodeGenEngine(String templatePath, HashMap content, String filePath,
			String fileName) throws Exception {
		return new HCodeGenEngine(CodeGeneratorFactory.getHCodeGenerator(templatePath), content, filePath, fileName);
	}
}
