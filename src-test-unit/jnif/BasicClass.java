package jnif;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.regex.*;
import java.util.*;

public class BasicClass {

	public static int hola2(int a, int b) {
		Pattern p = Pattern.compile("a*b");

		int c = a + b;
		return c;
	}

	private static void sort0(int[] array) {
		int n = array.length;

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

	private static void sort2() {
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

	private List listFactory0(int listClass) {
		List list;

		if (listClass == 0) {
			list = new ArrayList();
		} else if (listClass == 1) {
			list = new LinkedList();
		} else if (listClass == 2) {
			list = new Vector();
		} else if (listClass == 3) {
			list = new Stack();
		} else {
			throw new IllegalArgumentException("Invalid value for listClass");
		}

		return list;
	}

	private List<BasicClass> listFactory1(int listClass) {
		List<BasicClass> list;

		if (listClass == 0) {
			list = new ArrayList<BasicClass>();
		} else if (listClass == 1) {
			list = new LinkedList<BasicClass>();
		} else if (listClass == 2) {
			list = new Vector<BasicClass>();
		} else if (listClass == 3) {
			list = new Stack<BasicClass>();
		} else {
			throw new IllegalArgumentException("Invalid value for listClass");
		}

		list.add(new BasicClass());

		return list;
	}

}
