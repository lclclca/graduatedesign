package ima2c.utils;

import java.math.BigDecimal;

public class FloatUtil {
	public FloatUtil() {
	}

	public static float add(float... values) {
		BigDecimal res = new BigDecimal("0");
		float[] var5 = values;
		int var4 = values.length;

		for (int var3 = 0; var3 < var4; ++var3) {
			float f = var5[var3];
			BigDecimal bf = new BigDecimal(String.valueOf(f));
			res = res.add(bf);
		}

		return res.floatValue();
	}

	public static float subtract(float left, float right) {
		BigDecimal iLeft = new BigDecimal(String.valueOf(left));
		BigDecimal iRight = new BigDecimal(String.valueOf(right));
		return iLeft.subtract(iRight).floatValue();
	}

	public static float divide(float left, float right) {
		BigDecimal iLeft = new BigDecimal(String.valueOf(left));
		BigDecimal iRight = new BigDecimal(String.valueOf(right));

		try {
			return iLeft.divide(iRight).floatValue();
		} catch (Exception var5) {
			return iLeft.divide(iRight, 5).floatValue();
		}
	}

	public static int ceil_divide(float left, float right) {
		float resF = divide(left, right);
		return ((int) resF) != resF ? 1 + (int) resF : (int) resF;
	}

	public static float multiply(float left, float right) {
		BigDecimal iLeft = new BigDecimal(String.valueOf(left));
		BigDecimal iRight = new BigDecimal(String.valueOf(right));
		return iLeft.multiply(iRight).floatValue();
	}
}
