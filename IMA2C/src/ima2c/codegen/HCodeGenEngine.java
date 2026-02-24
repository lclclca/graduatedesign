package ima2c.codegen;

import java.io.IOException;
import java.util.HashMap;
import java.util.List;

import ima2c.utils.FileUtils;

public class HCodeGenEngine implements CodeGenEngine {

	CodeGenerator codeGen;
	HashMap<String, List> content;
	String filePath;
	String fileName;

	public HCodeGenEngine(CodeGenerator codeGen, HashMap<String, List> content, String filePath, String fileName) {
		super();
		this.codeGen = codeGen;
		this.content = content;
		this.filePath = filePath;
		this.fileName = fileName;
	}

	@Override
	public void setCodeGenerator(CodeGenerator codeGenerator) {
		this.codeGen = codeGenerator;
	}

	@Override
	public void genCode() {
		String srcCode = this.codeGen.codeGen(content);
		try {
			FileUtils.createAndWriteFile(filePath, fileName, srcCode);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	@Override
	public String getCode() {

		return null;
	}

}
