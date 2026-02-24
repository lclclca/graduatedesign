package ima2c.conf;

import java.net.URL;

import ima2c.Activator;

public class TemplateConf {

	private final static String TEMPLATE = "/template";

	private final static String CORE_XML_STG = "core.stg";

	private final static String ACTIVITY_C_STG = "activityc.stg";
	private final static String ACTIVITY_H_ST = "activityh.st";


	private final static String APERIODIC_C_ST = "aperiodic.c.stg";
	private final static String APERIODIC_H_ST = "aperiodic.h.st";


	private final static String MAIN_C_ST = "mainc.st";


	private final static String GLOBALS_H_ST = "globalsh.st";
	private final static String GLOBALS_C_ST = "globalsc.st";

	private final static String GTYPES_H_ST = "gtypesh.st";
	private final static String GTYPES_C_ST = "gtypesc.st";

	private final static String DEPLOYMENT_H_ST = "deploymenth.st";
	private final static String DEPLOYMENT_C_ST = "deploymentc.st";

	private final static String SUBPROGRAM_H_ST = "subprogramh.st";
	private final static String SUBPROGRAM_C_ST = "subprogramc.st";


	public static URL getTemplate(String fileName) {
		URL file_url = Activator.getDefault().getBundle().getEntry(TEMPLATE + fileName);
		System.out.println(file_url.getPath().toString());
		return file_url;
	}

	public static String getActivityCStg() {
		return TEMPLATE + "/" + ACTIVITY_C_STG;
	}

	public static String getActivityHSt() {
		return TEMPLATE + "/" + ACTIVITY_H_ST;
	}

	public static String getAperiodicCSt() {
		return TEMPLATE + "/" + APERIODIC_C_ST;
	}

	public static String getAperiodicHSt() {
		return TEMPLATE + "/" + APERIODIC_H_ST;
	}

	public static String getMainCSt() {
		return TEMPLATE + "/" + MAIN_C_ST;
	}

	public static String getGlobalsHSt() {
		return TEMPLATE + "/" + GLOBALS_H_ST;
	}

	public static String getGlobalsCSt() {
		return TEMPLATE + "/" + GLOBALS_C_ST;
	}

	public static String getGtypesHSt() {
		return TEMPLATE + "/" + GTYPES_H_ST;
	}

	public static String getGtypesCSt() {
		return TEMPLATE + "/" + GTYPES_C_ST;
	}

	public static String getDeploymentHSt() {
		return TEMPLATE + "/" + DEPLOYMENT_H_ST;
	}

	public static String getDeploymentCSt() {
		return TEMPLATE + "/" + DEPLOYMENT_C_ST;
	}

	public static String getSubprogramHSt() {
		return TEMPLATE + "/" + SUBPROGRAM_H_ST;
	}

	public static String getSubprogramCSt() {
		return TEMPLATE + "/" + SUBPROGRAM_C_ST;
	}

	public static String getCoreXmlStg() {
		return TEMPLATE + "/" + CORE_XML_STG;
	}

	public static URL getCoreXmlStgUrl() {
		return getTemplate(CORE_XML_STG);
	}

}
