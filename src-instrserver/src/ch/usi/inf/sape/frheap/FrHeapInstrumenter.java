package ch.usi.inf.sape.frheap;

import java.io.IOException;

public abstract class FrHeapInstrumenter {

	public abstract byte[] instrumentClass(byte[] classBytes, String className)
			throws IOException;

	public FrHeapInstrumentConfig config;

	public static boolean skipCompute(String className) {
		return className.startsWith("java/lang/");
		// || className.startsWith("sun");
	}
}
