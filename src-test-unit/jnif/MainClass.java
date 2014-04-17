package jnif;

import java.io.*;
import java.lang.reflect.*;

public class MainClass {

	public static void parseArgs(String[] args) throws IOException {
		int[] as = new int[34];

		switch (args.length) {
		case 0:
		case 1:
			System.out.println("args.length = " + args.length);
			break;
		case 2:
			System.out.println("args.length = 2");
			break;
		case 3:
			System.out.println("args.length = 3");
			break;
		case 4:
			System.out.println("args.length = 4");
			break;
		default:
			System.out.println("args.length unknown");
			break;
		}

		if (args.length > 0) {
			switch (args[0].charAt(0)) {
			case 'A':
				System.out.println("A");
				break;
			case 'B':
				System.out.println("B");
				break;
			case 'C':
				System.out.println("C");
				break;
			}
		}

		System.out.println("HeapTest: " + as[as.length - 1]);

		int i = 0;
		while (i < as.length) {
			as[i] = i;
			i++;
		}
	}
//
//	public static String showObject(Object o) {
//		if (o != null) {
//			String values = "";
//
//			Class<?> c = o.getClass();
//
//			for (Field f : c.getFields()) {
//				Object value = "?";
//				try {
//					value = f.get(o);
//				} catch (IllegalArgumentException e) {
//					e.printStackTrace();
//				} catch (IllegalAccessException e) {
//					e.printStackTrace();
//				}
//
//				if (!values.equals("")) {
//					values += ", ";
//				}
//
//				values += f.getName() + ": " + value;
//			}
//
//			String result = "{" + values + "}";
//
//			return result;
//		}
//
//		return "(null)";
//	}
}