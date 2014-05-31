package ch.usi.inf.sape.frheap;

import java.io.IOException;
import java.io.InputStream;

public abstract class FrHeapInstrumenter {

	public abstract byte[] instrumentClass(InputStream classBytes,
			String className) throws IOException;

	public FrHeapInstrumentConfig config;

}
