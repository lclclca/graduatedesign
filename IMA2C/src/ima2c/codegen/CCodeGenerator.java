package ima2c.codegen;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.stringtemplate.v4.ST;
import org.stringtemplate.v4.STGroup;

public class CCodeGenerator implements CodeGenerator {
	STGroup group;

	public CCodeGenerator(STGroup group) {
		super();
		this.group = group;
	}

	@Override
	public String codeGen(HashMap<String, List> content, ST core) {
		if (content == null) {
			return core.render();
		}
		for (Map.Entry<String, List> entry : content.entrySet()) {
			String key = (String) ((Map.Entry<?, ?>) entry).getKey();
			List val = (List) ((Map.Entry<?, ?>) entry).getValue();
			ST st = group.getInstanceOf(key);
			st.add(key, val);
			core.add(key, st);
		}

		return core.render();
	}

	@Override
	public void setCodeTemplate(STGroup template) {
		this.group = template;
	}

	@Override
	public ST getMainST(String parameter) {
		return group.getInstanceOf(parameter);
	}

	@Override
	public void setCodeTemplate(ST template) {
	}

	@Override
	public String codeGen(HashMap<String, List> content) {

		return null;
	}

}
