package frproxy;

public class FrInstrProxy {

	public static byte[] getResource(String className, ClassLoader loader) {
		//System.err.print("FrInstrProxy.getResource for ");
		//System.err.println(className);

		ClassLoader cl;
		if (loader != null) {
			cl = loader;
		} else {
			cl = ClassLoader.getSystemClassLoader();
		}

		//System.err.print("cl: ");
		//System.err.println(cl);

		java.io.InputStream is;
		try {
			is = cl.getResourceAsStream(className + ".class");
		} catch (Throwable e) {
			e.printStackTrace();
			return null;
		}

		//System.err.print("is: ");
		//System.err.println(is);
		try (java.io.ByteArrayOutputStream os = new java.io.ByteArrayOutputStream();) {
			byte[] buffer = new byte[0xFFFF];

			for (int len; (len = is.read(buffer)) != -1;) {
				os.write(buffer, 0, len);
			}

			os.flush();

			// System.err.println("Siamo finito getResource!!!");

			return os.toByteArray();
		} catch (Exception e) {
			// System.err.println("Errore in getResource");
			// e.printStackTrace();
			return null;
		}
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
