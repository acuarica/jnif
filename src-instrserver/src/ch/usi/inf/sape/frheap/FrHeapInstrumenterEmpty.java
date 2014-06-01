package ch.usi.inf.sape.frheap;

import java.io.IOException;

public class FrHeapInstrumenterEmpty extends FrHeapInstrumenter {

	@Override
	public byte[] instrumentClass(byte[] classBytes, String className)
			throws IOException {
		return classBytes;
	}

}
