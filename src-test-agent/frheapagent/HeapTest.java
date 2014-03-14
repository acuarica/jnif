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

	public static void main(String[] args) throws IOException {
		int[] as = new int[34];
		int[] as1 = new int[34];
		int[] as2 = new int[34];

		System.out.println("HeapTest: " + as.length);

		try {
			for (int i = 0; i < as.length; i++) {
				int[] arr = new int[i];

				// System.out.println("HeapTest: " + arr.length);
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
		// System.out.println("HeapTest: " + as[as.length - 1]);
	}
}
