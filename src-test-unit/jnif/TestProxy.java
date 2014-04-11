package jnif;

import java.lang.reflect.Field;

public class TestProxy {

	public static String showObject(Object o) {
		if (o != null) {
			String values = "";

			Class<?> c = o.getClass();

			for (Field f : c.getFields()) {
				Object value = "?";
				try {
					value = f.get(o);
				} catch (IllegalArgumentException e) {
					e.printStackTrace();
				} catch (IllegalAccessException e) {
					e.printStackTrace();
				}

				if (!values.equals("")) {
					values += ", ";
				}

				values += f.getName() + ": " + value;
			}

			String result = "{" + values + "}";

			return result;
		}

		return "(null)";
	}

	/**
	 * 
	 * @param count
	 * @param thisArray
	 * @param atype
	 */
	public static void newArrayEvent(int count, Object thisArray, int atype) {
		System.out.format("newArray | count: %d, thisArray: %s, atype: %d\n", count, thisArray, atype);
	}

	/**
	 * 
	 */
	public static void aNewArrayEvent(int count, Object thisArray, String type) {
		System.out.format("aNewArray | count: %d, thisArray: %s, type: %s\n", count, thisArray, type);
	}

	public static void multiANewArray1Event(int count1, Object thisArray, String type) {
		System.out.format("multiANewArray | count1: %d, thisArray: %s, type: %s\n", count1, thisArray, type);
	}

	public static void multiANewArray2Event(int count1, int count2, Object thisArray, String type) {
		System.out.format("multiANewArray | count1: %d, count2: %d, thisArray: %s, type: %s\n", count1, count2,
				thisArray, type);
	}

	public static void multiANewArrayNEvent(Object thisArray, int dims, String type) {
		System.out.format("multiANewArray | thisArray: %s, dims: %d, type: %s\n", thisArray, dims, type);
	}

	/**
	 * 
	 * @param thisObject
	 * @param newValue
	 * @param fieldName
	 */
	public static void putFieldEvent(Object thisObject, Object newValue, String fieldName) {
		System.out.format("putField | thisObject: %s, newValue: %s, fieldName: %s\n", thisObject, newValue, fieldName);
	}

	/**
	 * 
	 * @param newValue
	 * @param thisClassName
	 * @param fieldName
	 */
	public static void putStaticEvent(Object newValue, String thisClassName, String fieldName) {
		System.out.format("putStatic | newValue: %s, thisClassName: %s, fieldName: %s\n", newValue, thisClassName,
				fieldName);
	}

	public static void aastoreEvent(int index, Object newValue, Object thisArray) {
		System.out.format("aastoreEvent | index: %d, newValue: %s, thisArray: %s\n", index, newValue, thisArray);
	}
}
