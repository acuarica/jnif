package jnif;

public class TestAbs {

	static int iaload0() {
		int[] arr = null;

		if (arr == null) {
			arr = null;
		}

		return arr[1];
	}

	static float faload0() {
		float[] arr = null;

		if (arr == null) {
			arr = null;
		}

		return arr[1];
	}

	static Object aaload0() {
		Object[] arr = null;

		if (arr == null) {
			arr = null;
		}

		return arr[0];
	}

	static BasicClass aaload1() {
		BasicClass[] arr = null;

		if (arr == null) {
			arr = null;
		}

		return arr[0];
	}

	static Object aaload2(Object[] arrArg) {
		// int len;
		Object[] arr = null;

		if (arrArg != null) {
			// len = arrArg.length;
			// arr = null;
		} else {
			// len = 0;
			arr = null;
		}
		return arr[5];

		// for (int i = 0; i < 5; i++) {
		// System.out.println(arr[i]);
		// }
	}

	static long join0(long value) {
		long result;
		result = value;
		if (result < 0) {
			float c = 1.5f;
			result *= c;
		} else {
			int c = 2;
			result *= c;
		}

		return result;
	}

	static int abs0(int value) {
		int result;
		result = value;
		if (result < 0) {
			result = -result;
		}

		return result;
	}

	static int abs1(int value) {
		int result;
		result = value;
		if (result < 0) {
			int a = result;
			a = -a;
			result = a;
		}

		return result;
	}

	static boolean isPositive(int value) {
		boolean result;
		if (value >= 1) {
			result = true;
		} else {
			result = false;
		}
		return result;
	}

	int something(long l) {
		return 42;
	}

	int hola(int a, int b) {
		int c = a + b;
		return c;
	}

	static int sumWithWhile(int n) {
		int result = 0;

		int i = 1;
		while (i <= n) {
			result += i;
			i++;
		}

		return result;
	}

	static int sumWithFor(int n) {
		int result = 0;

		for (int i = 1; i <= n; i++) {
			result += i;
		}

		return result;
	}

	private static void m1(int a) {
		System.out.println("AB");
		System.out.println("AB");
		System.out.println("AB");
		System.out.println("AB");
		System.out.println("AB");
		System.out.println("AB");
		System.out.println("AB");
		System.out.println("AB");
		System.out.println("AB");
		System.out.println("AB");
		System.out.println("AB");

		if (a == 0) {
			System.out.print("Z");
		}
	}
}
