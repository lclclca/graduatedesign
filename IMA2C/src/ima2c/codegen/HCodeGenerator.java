package ima2c.codegen;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.stringtemplate.v4.ST;
import org.stringtemplate.v4.STGroup;

import ima2c.utils.FileUtils;

public class HCodeGenerator implements CodeGenerator {
	private ST template;

	public HCodeGenerator(String template) throws Exception {
		super();
		this.template = new ST(FileUtils.getTemplateByStream((template)));
	}

	@Override
	public String codeGen(HashMap<String, List> content, ST core) {
		return null;
	}

	@Override
	public String codeGen(HashMap<String, List> content) {
		if (content == null) {
			return template.render();
		}
		for (Map.Entry<String, List> entry : content.entrySet()) {
			String key = (String) ((Map.Entry<?, ?>) entry).getKey();
			List val = (List) ((Map.Entry<?, ?>) entry).getValue();
			template.add(key, val);
		}
		return template.render();
	}

	@Override
	public void setCodeTemplate(ST template) {
		this.template = template;
	}

	@Override
	public void setCodeTemplate(STGroup template) {
	}

	@Override
	public ST getMainST(String parameter) {
		return null;
	}

}
