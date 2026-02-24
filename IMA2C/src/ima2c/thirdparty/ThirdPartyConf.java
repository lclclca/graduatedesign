package ima2c.thirdparty;

import java.io.IOException;

import ima2c.conf.ExternalutilConf;

public class ThirdPartyConf {
	/**
	 * @Description: 以调用OpenGeode为例
	 * @param initFilePath void
	 * @author LSX
	 * @date 2022-09-09
	 */
	public static void loadOpenGeode(String initFilePath) {
		Runtime rn = Runtime.getRuntime();
		if (initFilePath == null) {
			initFilePath = "";
		}
		try {
			rn.exec(ExternalutilConf.OPENGEODE_PATH + " " + initFilePath);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
