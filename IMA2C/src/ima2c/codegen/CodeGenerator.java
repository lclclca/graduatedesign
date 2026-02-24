package ima2c.codegen;

import java.util.HashMap;
import java.util.List;

import org.stringtemplate.v4.ST;
import org.stringtemplate.v4.STGroup;

public interface CodeGenerator {

	public String codeGen(HashMap<String, List> content, ST core);

	public String codeGen(HashMap<String, List> content);

	public void setCodeTemplate(ST template);

	public void setCodeTemplate(STGroup template);

	public ST getMainST(String parameter);

}
