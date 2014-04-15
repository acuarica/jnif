package jnif;

import java.lang.reflect.*;
import java.util.*;
import java.util.regex.*;

public class TestProxy {

	public static int hola2(int a, int b) {
		Pattern p = Pattern.compile("a*b");

		int c = a + b;
		return c;
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
			list = new java.util.ArrayList<BasicClass>();
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

	public static void newArrayEvent(int count, Object thisArray, int atype) {
		System.out.format("newArray | count: %d, thisArray: %s, atype: %d\n",
				count, thisArray, atype);
	}

	public static void aNewArrayEvent(int count, Object thisArray, String type) {
		System.out.format("aNewArray | count: %d, thisArray: %s, type: %s\n",
				count, thisArray, type);
	}
}
