package ima2c.codegen;

public interface CodeGenEngine {

	public void setCodeGenerator(CodeGenerator codeGenerator);

	public void genCode();

	public String getCode();
}
