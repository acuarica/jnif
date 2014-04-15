package jnif;

public class TestAbs {

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
