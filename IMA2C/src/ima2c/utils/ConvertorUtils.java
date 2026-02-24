package ima2c.utils;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Pattern;

public class ConvertorUtils {

	public static String convertTime(String str) {
		if (str == null) {
			return null;
		}
		float value = getValue(str);
		String sourceUnit = getSourceUnit(str);
		return String.valueOf((long) convertInNs(value, sourceUnit));
	}

	public static float getValue(String str) {
		String regEx = "[^0-9]+";
		Pattern pattern = Pattern.compile(regEx);
		// 用定义好的正则表达式拆分字符串，把字符串中的数字留出来
		String number = pattern.split(str)[0];
		return Float.valueOf(number);
	}

	public static String getSourceUnit(String str) {
		String regEx = "[^0-9]+";
		Pattern pattern = Pattern.compile(regEx);
		// 用定义好的正则表达式拆分字符串，把字符串中的数字留出来
		String number = pattern.split(str)[0];
		String unit = str.replace(number, "");
		return unit;
	}

	public static float convertInNs(float value, String sourceUnit) {
		float ns;
		if (sourceUnit.equals("hr")) {
			ns = FloatUtil.multiply(value, 60.0F);
			return convertInNs(ns, "min");
		} else if (sourceUnit.equals("min")) {
			ns = FloatUtil.multiply(value, 60.0F);
			return convertInNs(ns, "sec");
		} else if (sourceUnit.equals("sec")) {
			ns = FloatUtil.multiply(value, 1000.0F);
			return convertInNs(ns, "ms");
		} else if (sourceUnit.equals("ms")) {
			ns = FloatUtil.multiply(value, 1000.0F);
			return convertInNs(ns, "us");
		} else if (sourceUnit.equals("us")) {
			ns = FloatUtil.multiply(value, 1000.0F);
			return ns;
		} else if (sourceUnit.equals("ns")) {
			return value;
		} else if (sourceUnit.equals("ps")) {
			ns = FloatUtil.divide(value, 1000.0F);
			return ns;
		} else {
			return value;
		}
	}

	/**
	 * 
	 * @Description: 用于时间的转换
	 * @param value
	 * @return double
	 * @author LSX
	 * @date 2022-06-01 04:01:59
	 */
	public static double timeConvert(String value) {
		// 正则表达式，用于匹配非数字串，+号用于匹配出多个非数字串
		String regEx = "[^0-9]+";
		Pattern pattern = Pattern.compile(regEx);
		// 用定义好的正则表达式拆分字符串，把字符串中的数字留出来
		String number = pattern.split(value)[0];
		// System.out.println("number:" + number);
		String unit = value.replace(number, "");
		// System.out.println("unit:" + unit);
		Double result = 0.0;
		switch (unit) {
		case "ps":
			result = psToSecond(Double.valueOf(number));
			break;
		case "ns":
			result = nsToSecond(Double.valueOf(number));
			break;
		case "us":
			result = usToSecond(Double.valueOf(number));
			break;
		case "ms":
			result = msToSecond(Double.valueOf(number));
			break;
		case "min":
			result = minToSecond(Double.valueOf(number));
			break;
		case "hr":
			result = hrToSecond(Double.valueOf(number));
			break;
		default:
			break;
		}
		return result;
	}

	private static Double psToSecond(Double value) {
		return nsToSecond(value) / 1000;
	}

	private static Double nsToSecond(Double value) {
		return usToSecond(value) / 1000;
	}

	private static Double usToSecond(Double value) {
		return msToSecond(value) / 1000;
	}

	private static Double msToSecond(Double value) {
		return value / 1000;
	}

	private static Double minToSecond(Double value) {
		return value * 60;
	}

	private static Double hrToSecond(Double value) {
		return minToSecond(value) * 60;
	}

	/**
	 * @Description: 内存大小转换,单位同意为Bytes
	 * @param str
	 * @return String
	 * @author LSX
	 * @date 2022-08-24
	 */
	public static String convertSize(String str) {
		String regEx = "[^0-9]+";
		Pattern pattern = Pattern.compile(regEx);
		// 用定义好的正则表达式拆分字符串，把字符串中的数字留出来
		String number = pattern.split(str)[0];
		String unit = str.replace(number, "");
		Long result = Long.valueOf(number);
		switch (unit) {
		case "bit":
			result = result / 8;
			break;
		case "Bytes":
			break;
		case "KByte":
			result = result * 1024;
			break;
		case "MByte":
			result = result * 1024 * 1024;
			break;
		case "GByte":
			result = result * 1024 * 1024 * 1024;
			break;
		case "TByte":
			result = result * 1024 * 1024 * 1024 * 1024;
			break;
		default:
			break;
		}
		return String.valueOf(result);
	}

	public static double addDouble(List<Double> data) {
		List<BigDecimal> bdList = new ArrayList<>();
		for (Double num : data) {
			bdList.add(new BigDecimal(num));
		}
		// populate list
		BigDecimal result = bdList.stream().reduce(BigDecimal.ZERO, BigDecimal::add);
		return result.doubleValue();
	}

}
