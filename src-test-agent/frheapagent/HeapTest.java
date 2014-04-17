package frheapagent;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.regex.*;

public class HeapTest {

	private boolean isCourseText;

	public int covariant(int arg, int size) {
		Object[] arr;

		if (arg == 0) {
			arr = new Integer[size];
		} else if (arg == 1) {
			arr = new Float[arg];
		} else {
			throw new RuntimeException("Invalid args!");
		}

		return arr.length;
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
		HeapTest ht = new HeapTest();
		System.out.println("Is course text?: " + ht.isCourseText());
		System.out.println("Really?: " + ht.isCourseText);

		int[] as = new int[34];
		int[] as1 = new int[34];
		int[] as2 = new int[34];

		System.out.println("HeapTest: " + as.length);

		try {
			for (int i = 0; i < as.length; i++) {
				int[] arr = new int[i];
			}

			for (int i = 0; i < 27; i++) {
				HeapTest[] arr = new HeapTest[i];
			}

			for (int i = 0; i < 27; i++) {
				HeapTest[] arr = new HeapTest[i];

				System.out.println("elem: " + arr[i]);

				System.out.println("HeapTest array len: " + arr.length);
			}

		} catch (Exception e) {
			System.out.println(e);
		}

		System.out.println("HeapTest: " + as.length);

		ts(args);
		ls(args);

		hola2(1, 2);
		// System.out.println("HeapTest: " + as[as.length - 1]);
	}
}
