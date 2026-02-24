package ima2c.utils;

import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;

public class ApiTest {

	public static void test2() {
		String core_connection_path = "src/dima2c/resourse/core-connection.xml.st";
		try {
			String core_connection = new String(Files.readAllBytes(Paths.get(core_connection_path)),
					Charset.defaultCharset());
			System.out.println(core_connection);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	public static void main(String[] args) {
		String projectRootDirectoryPath = System.getProperty("user.dir");
		System.out.println(projectRootDirectoryPath);

	}
}
