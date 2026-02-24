package ima2c.utils;

import java.util.HashMap;
import java.util.List;

import ima2c.entity.corexml.FeatureProperty;

public class ContentForeach {
	public static void displayAADLinfo(HashMap<String, String> content) {

		content.forEach((key, value) -> System.out.println("Key:" + key + ",Value:" + value));

		System.out.println();
	}

	public static void displayAADLListinfo(List<String> list) {

		list.forEach(s -> System.out.println(s));
		System.out.println();

	}

	public static void displayFeatureinfo(HashMap<String, FeatureProperty> threadFeatureContent) {
		// TODO Auto-generated method stub
		threadFeatureContent.forEach((key, value) -> System.out.println("Key:" + key + ",Value:" + value));

		System.out.println();
	}
}
