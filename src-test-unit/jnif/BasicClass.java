package jnif;

import java.io.*;
import java.util.regex.*;
import java.util.*;

public class BasicClass {

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
}
