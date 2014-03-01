package frproxy;

public class FrInstrProxy {

	public static native void alloc(Object thisObject);

	public static native void newArrayEvent(int count, Object thisArray, int atype);

	public static native void aNewArrayEvent(int count, Object thisArray, String type);

	public static native void multiANewArray1Event(int count1, Object thisArray, String type);

	public static native void multiANewArray2Event(int count1, int count2, Object thisArray, String type);

	public static native void multiANewArrayNEvent(Object thisArray, int dims, String type);

	public static native void putFieldEvent(Object thisObject, Object newValue, String fieldName);

	public static native void putStaticEvent(Object newValue, String thisClassName, String fieldName);

	public static native void aastoreEvent(int index, Object newValue, Object thisArray);

	public static native void enterMainMethod();

	public static native void exitMainMethod();

}
