package frheapagent;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.*;
import java.util.regex.*;

public class HeapTest {

	private boolean isCourseText;

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

	private static void use(Object arr) {

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

			// throw e;
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

	// public int covariant(int arg, int size) {
	// Object[] arr;
	//
	// if (arg == 0) {
	// arr = new ArrayList[size];
	// } else if (arg == 1) {
	// arr = new Vector[arg];
	// } else {
	// throw new RuntimeException("Invalid args!");
	// }
	//
	// return arr.length;
	// }

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

	public static void main(String[] args) throws IOException {
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
