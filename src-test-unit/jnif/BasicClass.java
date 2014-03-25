package jnif;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.regex.*;

public class BasicClass {

	int something(long l) {
		return 42;
	}

	int hola(int a, int b) {
		int c = a + b;
		return c;
	}

	public static int hola2(int a, int b) {
		Pattern p = Pattern.compile("a*b");

		int c = a + b;
		return c;
	}

	private static void sort() {
		int n = 50;

		int[] array = new int[n];

		for (int i = 0; i < n; i++) {
			array[i] = i * 2;
		}

		for (int c = 0; c < n - 1; c++) {
			for (int d = 0; d < n - c - 1; d++) {
				if (array[d] > array[d + 1]) {
					int swap = array[d];
					array[d] = array[d + 1];
					array[d + 1] = swap;
				}
			}
		}
	}

	public static void main(String[] args) throws IOException {
		int[] as = new int[34];

		for (int i = 0; i < as.length; i++) {
			as[i] = i;
		}

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

		hola2(1, 2);

		System.out.println("HeapTest: " + as[as.length - 1]);
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
