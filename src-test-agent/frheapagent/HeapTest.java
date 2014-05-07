package frheapagent;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.*;
import java.util.regex.*;

public class HeapTest {

	private boolean isCourseText;

	private static boolean eq(Object o1, Object o2) {
		return o1 == null ? o2 == null : o1.equals(o2);
	}

	public void method() {
		System.out.println("HeapTest.method");
	}

	private static void if0(boolean arg) {
		HeapTest o = null;

		if (arg) {
			o = null;
		}

		o.method();
	}

	private static boolean generic0(ArrayList<String> arr) {
		boolean res;

		if (arr.size() == 0) {
			res = true;
		} else {
			res = false;
		}

		return res;
	}

	private static int for0(int n) {
		int sum = 0;
		for (int i = 0; i < 1; i++) {
			sum += i;
		}

		return sum;
	}

	private static Object for1() {
		Object obj = null;
		for (int i = 0; i < 4; i++) {
			obj = new Base();
		}

		if (obj == null) {
			obj = new Base();
		}

		return obj;
	}

	private static Object for2() {
		Object obj = null;
		for (int i = 0; i < 4; i++) {
			obj = new Derived1();
		}

		if (obj == null) {
			obj = new Base();
		}

		return obj;
	}

	private static Object self0(int arg) {
		Object obj = new HeapTest();
		if (arg == 0) {
			obj = new HeapTest();
		}

		return obj;
	}

	private static Object self1(int arg) {
		Object obj = null;
		if (arg == 0) {
			obj = new HeapTest();
		}

		return obj;
	}

	// private static Object self1(int arg) {
	// Object obj = new Object();
	// if (arg == 0) {
	// obj = new HeapTest();
	// }
	//
	// return obj;
	// }

	private static void scope0(int arg) {
		{
			int o = arg;

			if (arg == 0) {
				System.out.println("0");
			}

			System.out.println(o);
		}

		{
			String o = "";

			if (arg == -1) {
				System.out.println("-1");
			}

			System.out.println(o);
		}
	}

	private static void whileTrue() {
		int n = 0;
		while (true) {
			n++;
		}
	}

	private static void checkcast0() throws CloneNotSupportedException {
		for (int i = 0; i < 10; i++) {
			HeapTest obj1 = (HeapTest) new HeapTest().clone();
		}
	}

	private static void checkcast1() throws CloneNotSupportedException {
		HeapTest[] arr1 = (HeapTest[]) new HeapTest[20].clone();

		for (int i = 0; i < 10; i++) {
			use(arr1[i]);
		}
	}

	private static void use(Object obj) {
		if (obj == null) {
			System.err.println("(null)");
		} else {
			Class<?> clazz = obj.getClass();
			System.err.println(clazz);
		}
	}

	private static void arrays0(int size) {
		use(new boolean[size]);
		use(new byte[size]);
		use(new char[size]);
		use(new short[size]);
		use(new int[size]);
		use(new float[size]);
		use(new long[size]);
		use(new double[size]);

		use(new Object[size]);
		use(new Byte[size]);
		use(new Integer[size]);
		use(new HeapTest[size]);

		use(new boolean[size][size]);
		use(new byte[size][size]);
		use(new char[size][size]);
		use(new short[size][size]);
		use(new int[size][size]);
		use(new float[size][size]);
		use(new long[size][size]);
		use(new double[size][size]);

		use(new Object[size][size]);
		use(new Byte[size][size]);
		use(new Integer[size][size]);
		use(new HeapTest[size][size]);

		use(new short[size][size][size]);
		use(new int[size][size][size]);

		short[][] arr0 = new short[size][];
		for (int i = 0; i < size; i++) {
			arr0[i] = new short[size];

			short[] innerArray = arr0[i];
			for (int j = 0; j < size; j++) {
				short value = innerArray[i];
				use(value);
			}
		}
	}

	private static void arrays1(int size) {

		HeapTest[][] arr0 = new HeapTest[size][];
		for (int i = 0; i < size; i++) {
			arr0[i] = new HeapTest[size];

			HeapTest[] innerArray = arr0[i];
			for (int j = 0; j < size; j++) {
				HeapTest value = innerArray[i];
				use(value);
			}
		}
	}

	private static boolean char0(int size) {
		char[] array = new char[size];

		int n = array.length;

		boolean res;
		if (n == 0) {
			res = false;
		} else {
			res = true;
		}

		return res;
	}

	private static Object nullArray0(boolean isNull) {
		Object[] arr;

		if (isNull) {
			arr = null;
		} else {
			arr = new Object[10];
		}

		return arr[0];
	}

	private static Object nullArray1(boolean isNull) {
		Object[] arr = null;

		if (isNull) {
			arr = null;
		}

		return arr[0];
	}

	private static double nullArray2(boolean isNull) {
		double[] arr;

		if (isNull) {
			arr = null;
		} else {
			arr = new double[10];
		}

		return arr[0];
	}

	private static double nullArray3(boolean isNull) {
		double[] arr = null;

		if (isNull) {
			arr = null;
		}

		return arr[0];
	}

	private static HeapTest tryCatch0(HeapTest arg)
			throws CloneNotSupportedException {
		HeapTest obj0;
		HeapTest obj1;
		try {
			obj0 = (HeapTest) new HeapTest().clone();
			obj1 = (HeapTest) arg.clone();
		} catch (Exception e) {
			e.printStackTrace();
			obj0 = null;
			obj1 = null;

			// throw e;
		}

		return obj0;
	}

	private static HeapTest tryCatch1(HeapTest arg)
			throws CloneNotSupportedException {
		HeapTest obj0;
		System.out.println("Enter method tryCatch1");
		try {
			obj0 = (HeapTest) new HeapTest().clone();
		} catch (Exception e) {
			e.printStackTrace();
			obj0 = null;
		}

		return obj0;
	}

	private static HeapTest tryCatch2(HeapTest arg)
			throws CloneNotSupportedException {
		HeapTest obj0;
		HeapTest obj1;
		HeapTest obj2 = null;
		HeapTest obj3 = null;

		if (arg == null) {
			arg = obj2;
		} else {
			arg = obj3;
		}

		try {
			obj0 = (HeapTest) new HeapTest().clone();
			obj1 = (HeapTest) arg.clone();
		} catch (Exception e) {
			e.printStackTrace();
			obj0 = null;
			obj1 = null;
		}

		return obj0;
	}

	private static HeapTest tryCatch3(HeapTest arg, int size)
			throws CloneNotSupportedException {
		HeapTest obj0;
		System.out.println("Enter method tryCatch1");
		int n = size;
		try {
			do {
				obj0 = (HeapTest) new HeapTest().clone();
				n--;
			} while (n > 0);
		} catch (Exception e) {
			e.printStackTrace();
			obj0 = null;
		}

		return obj0;
	}

	private static AbstractList tryCatch4() {
		try {
			return new Vector();
		} catch (Throwable e) {
			return null;
		}
	}

	private static HeapTest tryCatch5() {
		HeapTest o = null;
		try {
			return o;
		} catch (Throwable e) {
			return o;
		}
	}

	private static HeapTest tryCatch6() {
		HeapTest o = null;
		try {
			o = new HeapTest();
			return o;
		} catch (Throwable e) {
			return o;
		}
	}

	private static HeapTest tryCatch7() {
		HeapTest o = null;
		try {
			o = new HeapTest();
			return o;
		} catch (Throwable e) {
			e.printStackTrace();
			return o;
		}
	}

	private static HeapTest tryCatchFinally(HeapTest arg, int size)
			throws CloneNotSupportedException {
		HeapTest obj0;
		System.out.println("Enter method tryCatch1");
		int n = size;
		try {
			do {
				obj0 = (HeapTest) new HeapTest().clone();
				n--;
			} while (n > 0);
		} catch (Exception e) {
			e.printStackTrace();
			obj0 = null;
		} finally {
			System.out.print("finally!");
		}

		return obj0;
	}

	private static Object hierarchy0(int arg, int size) {
		Object obj;

		if (arg == 0) {
			obj = new ArrayList();
		} else if (arg == 1) {
			obj = new Vector();
		} else {
			throw new RuntimeException("Invalid args!");
		}

		return obj;
	}

	public int covariant0(int arg, int size) {
		Object[] arr;

		if (arg == 0) {
			arr = new ArrayList[size];
		} else if (arg == 1) {
			arr = new Vector[arg];
		} else {
			throw new RuntimeException("Invalid args!");
		}

		return arr.length;
	}

	public int covariant1(int arg, int size) {
		Object[] arr;

		if (arg == 0) {
			arr = new ArrayList[size];
		} else if (arg == 1) {
			arr = new Vector[arg];
		} else {
			throw new RuntimeException("Invalid args!");
		}

		return arr.length;
	}

	private static void sort0(int[] array) {
		int n = array.length;

		int c = 0;
		while (c < n - 1) {
			for (int d = 0; d < n - c - 1; d++) {
				if (array[d] > array[d + 1]) {
					int swap = array[d];
					array[d] = array[d + 1];
					array[d + 1] = swap;
				}
			}

			c++;
		}
	}

	private static void sort1() {
		int n = 50;

		int[] array = new int[n];

		int i = 0;
		while (i < n) {
			array[i] = i * 2;
			i++;
		}

		int c = 0;
		while (c < n - 1) {
			for (int d = 0; d < n - c - 1; d++) {
				if (array[d] > array[d + 1]) {
					int swap = array[d];
					array[d] = array[d + 1];
					array[d + 1] = swap;
				}
			}

			c++;
		}
	}

	int something(long l) {
		return 42;
	}

	int hola(int a, int b) {
		int c = a + b;
		return c;
	}

	public boolean isCourseText() {
		return isCourseText;
	}

	public static int hola2(int a, int b) {
		Pattern p = Pattern.compile("a*b");

		int c = a + b;
		return c;
	}

	double dhola(double a) {
		double b = 2;
		double c = a + b;
		return c;
	}

	public static void ts(String[] args) {
		switch (args.length) {
		case 0:
			HeapTest[] arr0 = new HeapTest[0];
			break;
		case 1:
			HeapTest[] arr1 = new HeapTest[1];
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
	}

	public static void ls(String[] args) {
		switch (args.length) {
		case 0:
			HeapTest[] arr0 = new HeapTest[0];
			break;
		case 1000:
			HeapTest[] arr1000 = new HeapTest[1000];
			break;
		case 2000:
			HeapTest[] arr2000 = new HeapTest[2000];
			System.out.println("args.length = " + args.length);
			break;
		case 3000:
			System.out.println("args.length = 3000");
			break;
		case 4000:
			System.out.println("args.length = 4000");
			break;
		case 5000:
			System.out.println("args.length = 5000");
			break;
		default:
			System.out.println("args.length unknown");
			break;
		}
	}

	// private static Object inner0(int arg) {
	// HashMap<String, String> map = new HashMap<String, String>();
	//
	// Object obj = null;
	// for (Map.Entry<String, String> entry : map.entrySet()) {
	// obj = entry;
	// }
	//
	// if (obj == null) {
	// obj = new Object();
	// }
	//
	// return obj;
	// }

	public static Base getObject(int arg) {
		Base res;
		if (arg == 0) {
			res = new Derived1();
		} else {
			// if (arg == 1) {
			res = new Derived2();
			// } else {
			// res = null;
		}

		return res;
	}

	public static class TestExc1 extends Exception {
	}

	public static class TestExc2 extends Exception {
	}

	public static void tryItOut() throws TestExc1, TestExc2 {
	}

	public static void handleExc1(Exception e) {
	}

	public static void handleExc2(Exception e) {
	}

	public static void nestedCatch0() {
		try {
			try {
				tryItOut();
			} catch (TestExc1 e) {
				handleExc1(e);
			}
		} catch (TestExc2 e) {
			handleExc2(e);
		}
	}

	public static void useFloat0() {
		float f = 14;

		System.err.println(f);
	}

	public static void main(String[] args) throws IOException {
		use(new Chapter3Snippets());
		use(new Chapter3Snippets().new Far());
		use(new Chapter3Snippets.TestExc1());
		use(new Chapter3Snippets.Example());

		tryCatch7();

		// useFloat0();

		// use(java.util.regex.Pattern.compile(""));

		// HeapTest ht = new HeapTest();
		// System.out.println("Is course text?: " + ht.isCourseText());
		// System.out.println("Really?: " + ht.isCourseText);
		//
		// int[] as = new int[34];
		// int[] as1 = new int[34];
		// int[] as2 = new int[34];
		//
		// System.out.println("HeapTest: " + as.length);
		//
		// try {
		// for (int i = 0; i < as.length; i++) {
		// int[] arr = new int[i];
		// }
		//
		// for (int i = 0; i < 27; i++) {
		// HeapTest[] arr = new HeapTest[i];
		// }
		//
		// for (int i = 0; i < 27; i++) {
		// HeapTest[] arr = new HeapTest[i];
		// System.out.println("elem: " + arr[i]);
		// System.out.println("HeapTest array len: " + arr.length);
		// }
		//
		// } catch (Exception e) {
		// System.out.println(e);
		// }
		//
		// System.out.println("HeapTest: " + as.length);
		//
		// ts(args);
		// ls(args);
		//
		// hola2(1, 2);
		// System.out.println("HeapTest: " + as[as.length - 1]);
	}

}
