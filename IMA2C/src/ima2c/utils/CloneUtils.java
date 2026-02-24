package ima2c.utils;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

/**
 * @Description: 深拷贝工具类
 * @author LSX
 */
public class CloneUtils {
	public static <T extends Serializable> T clone(T obj) {
		T cloneObj = null;
		try {
			// 写入字节
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
			ObjectOutputStream oos = new ObjectOutputStream(baos);
			oos.writeObject(obj);
			oos.close();
			// 分配内存,写入原始对象，生成新对象
			ByteArrayInputStream bais = new ByteArrayInputStream(baos.toByteArray());// 获取上面的输出字节
			ObjectInputStream ois = new ObjectInputStream(bais);
			// 返回生成的新对象
			cloneObj = (T) ois.readObject();
			ois.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return cloneObj;
	}

}
