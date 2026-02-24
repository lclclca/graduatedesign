package ima2c.utils;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.io.IOUtils;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.resource.URIConverter;
import org.eclipse.xtext.resource.ClasspathUriResolutionException;

import ima2c.popup.actions.Aadl2CAction;

/**
 * @author LSX
 */
public class FileUtils {

	/**
	 * @Description 根据指定路径创建文件夹
	 * @Date 17:10 2022/4/20
	 * @param: path
	 */
	public static void createFolder(String path) {
		try {
			File folder = new File(path);
			if (!folder.exists()) {
				folder.mkdirs(); // 创建目录
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("create floder error!");
		}
	}

	/**
	 * @Description: 通过类加载器加载文件内容
	 * @param template 文件的相对路径 "/template/activityh.st"
	 * @return
	 * @throws IOException String
	 * @author LSX
	 * @date 2023-02-26
	 */
	public static String getTemplateByStream(String template) throws IOException {
		InputStream stream = Aadl2CAction.class.getClassLoader().getResourceAsStream(template);
		return IOUtils.toString(stream, Charset.defaultCharset());
	}

	/**
	 * @Description 根据指定路径创建文件夹
	 * @Date 17:10 2022/4/20
	 * @param: path
	 */
	public static void createFolder(String parent, String child) {
		try {
			File folder = new File(parent + "\\" + child);
			if (!folder.exists()) {
				folder.mkdirs(); // 创建目录
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("create floder error!");
		}
	}

	public static void copyFile(File source, String dest) throws IOException {
		// 创建目的地文件夹
		File destfile = new File(dest);
		if (!destfile.exists()) {
			destfile.mkdir();
		}
		// 如果source是文件夹，则在目的地址中创建新的文件夹
		if (source.isDirectory()) {
			File file = new File(dest + "\\" + source.getName());// 用目的地址加上source的文件夹名称，创建新的文件夹
			file.mkdir();
			// 得到source文件夹的所有文件及目录
			File[] files = source.listFiles();
			if (files.length == 0) {
				return;
			} else {
				for (int i = 0; i < files.length; i++) {
					copyFile(files[i], file.getPath());
				}
			}

		}
		// source是文件，则用字节输入输出流复制文件
		else if (source.isFile()) {
			FileInputStream fis = new FileInputStream(source);
			// 创建新的文件，保存复制内容，文件名称与源文件名称一致
			File dfile = new File(dest + "\\" + source.getName());
			if (!dfile.exists()) {
				dfile.createNewFile();
			}

			FileOutputStream fos = new FileOutputStream(dfile);
			// 读写数据
			// 定义数组
			byte[] b = new byte[1024];
			// 定义长度
			int len;
			// 循环读取
			while ((len = fis.read(b)) != -1) {
				// 写出数据
				fos.write(b, 0, len);
			}

			// 关闭资源
			fos.close();
			fis.close();

		}
	}

	// filePath: "D:/a/b"
	// fileName: filename.txt
	// 也可以使用common.io的FileUtils.writeStringToFile
	public static void createAndWriteFile(String filePath, String fileName, String content) throws IOException {

		File dir = new File(filePath);
		// 一、检查放置文件的文件夹路径是否存在，不存在则创建
		if (!dir.exists()) {
			dir.mkdirs();// mkdirs创建多级目录
		}
		File checkFile = new File(filePath + "/" + fileName);
		FileWriter writer = null;
		try {
			// 二、检查目标文件是否存在，不存在则创建
			if (!checkFile.exists()) {
				checkFile.createNewFile();// 创建目标文件
			}
			// 三、向目标文件中写入内容
			// FileWriter(File file, boolean append)，
			// append为true时为追加模式，
			// false或缺省则为覆盖模式
			writer = new FileWriter(checkFile, false);
			writer.append(content);
			writer.flush();
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			if (null != writer) {
				writer.close();
			}
		}
	}

	/**
	 * This method checks that files stored in an array of path (String) actually
	 * exist.
	 * 
	 * @param fileNamesArray the array fo String referencing files.
	 * @param faultyFiles    the files not found
	 * @return a set of #{@link java.io.File} identified from the path in
	 *         {@code includeDirArray}.
	 */
	public static Set<File> checkFilesExist(String fileNamesArray[], boolean[] faultyFiles) {
		Set<File> includeDirSet = new LinkedHashSet<File>();
		for (int i = 0; i < fileNamesArray.length; i++) {
			File potentialFile = new File(fileNamesArray[i]);
			if (potentialFile.exists()) {
				includeDirSet.add(potentialFile);
				faultyFiles[i] = false;
				continue;
			} else {
				faultyFiles[i] = true;
			}
		}
		return includeDirSet;
	}

	/**
	 * This method deletes a file or a directory. If {@code file} is a directory,
	 * its content is deleted recursively and then it is deleted as well.
	 * 
	 * @param file the file or directory that must be deleted.
	 */
	public static void deleteDirectory(File file) {

		if (file.isDirectory()) {
			for (File subFile : file.listFiles())
				deleteDirectory(subFile);
			file.delete();
		} else
			file.delete();
	}

	/**
	 * This method create a new #{@link java.io.File}, puts the String
	 * {@code content} in this {@code file}, and saves it.
	 * 
	 * @param file    the destination in which {@code content} must be saved.
	 * @param content the String that identifies the future content of the file
	 * @return true if the creation of the file succeeds, false otherwise.
	 */
	public static boolean createFile(File file, String content) {
		PrintWriter writer = null;
		try {
			if (file.createNewFile()) {
				writer = new PrintWriter(file);
				writer.write(content);
			}
		} catch (IOException e) {
			return false;
		} finally {
			if (writer != null)
				writer.close();
		}
		return true;
	}

	/**
	 * From a directory, this method looks recursively for files with a given
	 * extension.
	 * 
	 * @param file              the root directory from which files with extension
	 *                          {@code filenameExtension} will be retrieved.
	 * @param filenameExtension the file extension to identified files of interest.
	 * @return the list of File found with the extension {@code filenameExtension}.
	 */
	public static List<File> findFiles(File file, String filenameExtension) {

		List<File> paths = new ArrayList<File>();
		List<File> files = new ArrayList<File>();

		if (file.isDirectory() == false)
			return null;

		files.add(file);

		while (files.isEmpty() == false) {

			File tmpFile = files.remove(files.size() - 1);

			for (File subFile : tmpFile.listFiles())
				if (subFile.isDirectory())
					files.add(subFile);
				else if (subFile.getName().endsWith(filenameExtension)) {
					paths.add(subFile);
				}
		}

		return paths;
	}

	/**
	 * This method copies the content of a file (possibly directory) in another
	 * file. Both must be created before calling this method.
	 * 
	 * @param src  the source file that will be copied.
	 * @param dest the destination file, result of the copy.
	 * @throws for any IO problems
	 */
	public static void copyFile(File src, File dest) throws IOException {
		InputStream in = null;
		OutputStream out = null;

		if (dest.isDirectory())
			dest = new File(dest, src.getName());

		try {
			in = new FileInputStream(src);
			out = new FileOutputStream(dest);

			byte[] buf = new byte[1024];
			int len;
			while ((len = in.read(buf)) > 0) {
				out.write(buf, 0, len);
			}
		} finally {
			if (in != null) {
				in.close();
			}

			if (out != null) {
				out.close();
			}
		}
	}

	/**
	 * This method copies, from a given directory, a set of files with a given list
	 * of file extensions.
	 * 
	 * @param src               the root directory from which files are copied.
	 * @param dest              the destination directory in which files are copied.
	 * @param filenameExtension a list of file extension to find which file to copy.
	 */
	public static void copyFiles(File src, File dest, String... filenameExtension) throws IOException {

		List<File> files = new ArrayList<File>();

		if (src.isDirectory() == false) {
			return;
		} else {
			files.add(src);

			while (files.isEmpty() == false) {

				File tmpFile = files.remove(files.size() - 1);

				for (File subFile : tmpFile.listFiles()) {
					if (subFile.isDirectory()) {
						files.add(subFile);
					} else {
						for (String extension : filenameExtension) {
							if (subFile.getName().endsWith(extension)) {
								copyFile(subFile, new File(dest, subFile.getName()));
							}
						}
					}
				}
			}
		}
	}

	/**
	 * This method save the String {@code content} in a file which name is
	 * {@code fileName}. The file is saved in directory {@code directory}.
	 * 
	 * @param directory the directory in which new file will be saved.
	 * @param fileName  the name of the created file.
	 * @param content   the content saved in the created file.
	 * @throws IOException {@link IOException}
	 */
	public static void saveFile(final File directory, final String fileName, final String content) throws IOException {
		BufferedWriter output;

		FileWriter file = new FileWriter(directory.getAbsolutePath() + File.separator + fileName);

		output = new BufferedWriter(file);

		output.write(content);

		output.close();
	}

	public static void saveFile(final URI directory, final String fileName, final String content) throws IOException {
		saveFile(createFile(directory), fileName, content);
	}

	public static String getFileInBuffer(String filename) throws IOException {
		BufferedReader reader = new BufferedReader(new FileReader(new File(filename)));
		String buffer = "";
		String line = "";

		while ((line = reader.readLine()) != null) {
			buffer += line + "\n";
		}

		reader.close();
		return buffer;
	}

	public static boolean makeDir(final URI uri) {
		return createFile(uri).mkdir();
	}

	private static File createFile(final URI uri) {
		final String path;

		if (uri.isFile()) {
			path = uri.toFileString();
		} else {
			path = uri.path();
		}

		return new File(path);
	}

	public static boolean createNewFile(final URI uri) throws IOException {
		return createFile(uri).createNewFile();
	}

	public static URI removeScheme(final URI uri) {
		return URI.createURI(uri.path());
	}

	public static boolean exists(final URI uri, final URIConverter uriConverter) {
		try {
			return uriConverter.exists(uri, null);
		} catch (final ClasspathUriResolutionException ex) {
			return false;
		}
	}

	public static boolean isDirectory(final URI uri, final URIConverter uriConverter) {
		final Map<String, ?> att = uriConverter.getAttributes(uri, null);

		return Boolean.TRUE.equals(att.get(URIConverter.ATTRIBUTE_DIRECTORY));
	}

}
