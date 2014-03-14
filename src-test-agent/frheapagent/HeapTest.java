package frheapagent;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class HeapTest {

	int something(long l) {
		return 42;
	}

	int hola(int a, int b) {
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
		// System.out.println("HeapTest: " + as[as.length - 1]);
	}
}
