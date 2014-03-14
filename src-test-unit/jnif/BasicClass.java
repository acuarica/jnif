package jnif;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class BasicClass {

	int something(long l) {
		return 42;
	}

	int hola(int a, int b) {
		int c = a + b;
		return c;
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

		System.out.println("HeapTest: " + as[as.length - 1]);
	}
}
