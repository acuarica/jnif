package frproxy;

public class FrInstrProxy {

	private static byte[] getResourceEx(String className, ClassLoader loader) {
		java.io.InputStream is;

		try {
			is = loader.getResourceAsStream(className + ".class");
		} catch (Throwable e) {
			// System.err.println("Error: getResource for " + className);
			// System.err.println("Error: loader: " + loader);
			// e.printStackTrace();
			return null;
		}

		java.io.ByteArrayOutputStream os = new java.io.ByteArrayOutputStream();
		try {
			byte[] buffer = new byte[0xFFFF];

			for (int len; (len = is.read(buffer)) != -1;) {
				os.write(buffer, 0, len);
			}

			os.flush();

			return os.toByteArray();
		} catch (Throwable e) {
			// System.err.println("Error: getResource for " + className);
			// System.err.println("Error: loader: " + loader);
			// System.err.println("Error: is: " + is);
			// e.printStackTrace();
			return null;
		}
	}

	public static byte[] getResource(String className, ClassLoader loader) {
		byte[] res = null;

		if (loader != null) {
			res = getResourceEx(className, loader);
		}

		if (res == null) {
			ClassLoader cl = ClassLoader.getSystemClassLoader();
			res = getResourceEx(className, cl);
		}

		// if (res == null) {
		// String binaryName = className.replace('/', '.');
		// try {
		// Class<?> cls = loader.loadClass(binaryName);
		// } catch (Throwable e) {
		// System.err
		// .println("Class not found even with loadClass for class: "
		// + className + "(" + binaryName + ")");
		// e.printStackTrace();
		// }
		// }

		return res;
	}

	public static native void alloc(Object thisObject);

	public static native void newArrayEvent(int count, Object thisArray,
			int atype);

	public static native void aNewArrayEvent(int count, Object thisArray,
			String type);

	public static native void multiANewArray1Event(int count1,
			Object thisArray, String type);

	public static native void multiANewArray2Event(int count1, int count2,
			Object thisArray, String type);

	public static native void multiANewArrayNEvent(Object thisArray, int dims,
			String type);

	public static native void putFieldEvent(Object thisObject, Object newValue,
			String fieldName);

	public static native void putStaticEvent(Object newValue,
			String thisClassName, String fieldName);

	public static native void aastoreEvent(int index, Object newValue,
			Object thisArray);

	public static native void enterMethod(String className, String methodName);

	public static native void enterMainMethod();

	public static native void exitMainMethod();

}
