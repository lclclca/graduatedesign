package ima2c.codegen;

import java.io.IOException;
import java.util.HashMap;
import java.util.List;

import org.stringtemplate.v4.ST;

import ima2c.utils.FileUtils;

public class CCodeGenEngine implements CodeGenEngine {
	CodeGenerator codeGen;
	HashMap<String, List> content;
	String fileName;
	String filePath;
	// 这个必须是最顶层的ST
	ST template;

	public CCodeGenEngine(CodeGenerator codeGen, HashMap<String, List> content, String fileName, String filePath,
			String parameter) {
		super();
		this.codeGen = codeGen;
		this.content = content;
		this.fileName = fileName;
		this.filePath = filePath;
		this.template = codeGen.getMainST(parameter);
	}

	public void setTemplate(ST template) {
		this.template = template;
	}

	@Override
	public void setCodeGenerator(CodeGenerator codeGenerator) {
		this.codeGen = codeGenerator;
	}

	@Override
	public void genCode() {
		String srcCode = this.codeGen.codeGen(content, template);
		try {
			FileUtils.createAndWriteFile(filePath, fileName, srcCode);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	@Override
	public String getCode() {
		return this.codeGen.codeGen(content, template);
	}

}
