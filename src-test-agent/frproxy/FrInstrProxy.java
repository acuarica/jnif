package frproxy;

public class FrInstrProxy {

	public static byte[] getResource(String className, ClassLoader loader) {
		System.err.print("Siamo arrivati a getResource0: ");
		System.err.println(className);

		ClassLoader cl;
		if (loader != null) {
			cl = loader;
		} else {
			cl = ClassLoader.getSystemClassLoader();
		}

		java.io.InputStream is = loader.getResourceAsStream(className
				+ ".class");

		System.err.print("is: ");
		System.err.println(is);
		try (java.io.ByteArrayOutputStream os = new java.io.ByteArrayOutputStream();) {
			byte[] buffer = new byte[0xFFFF];

			for (int len; (len = is.read(buffer)) != -1;) {
				os.write(buffer, 0, len);
			}

			os.flush();

			System.err.println("Siamo finito getResource!!!");

			return os.toByteArray();
		} catch (Exception e) {
			System.err.println("Errore in getResource");
			e.printStackTrace();
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
