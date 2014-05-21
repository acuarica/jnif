package ch.usi.inf.sape.frheap.tests;

import java.util.Arrays;
import java.util.LinkedList;

public class BaseCases {

	public static class Entry {
		public Object value;

		public Entry(Object obj) {
			value = obj;
		}

		@Override
		public String toString() {
			if (value != null) {
				System.out.println(value.getClass().getName());
				if (value.getClass().isArray()) {
					Class<?> t = value.getClass().getComponentType();
					if (t == boolean.class) {
						return Arrays.toString((boolean[]) value);
					} else if (t == char.class) {
						return Arrays.toString((char[]) value);
					} else if (t == byte.class) {
						return Arrays.toString((byte[]) value);
					} else if (t == short.class) {
						return Arrays.toString((short[]) value);
					} else if (t == int.class) {
						return Arrays.toString((int[]) value);
					} else if (t == long.class) {
						return Arrays.toString((long[]) value);
					} else if (t == float.class) {
						return Arrays.toString((float[]) value);
					} else if (t == double.class) {
						return Arrays.toString((double[]) value);
					} else {
						return Arrays.deepToString((Object[]) value);
					}
				} else {
					return value.toString();
				}
			} else {
				return "(--null--)";
			}
		}
	}

	private final LinkedList<Entry> list = new LinkedList<Entry>();

	protected <T> T use(T o) {
		System.out.println("Using: " + o);
		list.add(new Entry(o));

		return o;
	}

	public LinkedList<Entry> getMap() {
		return list;
	}
}
