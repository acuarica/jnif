package ch.usi.inf.sape.frheap;

public class FrHeapInstrumentConfig {

	String proxyClass;

	String dumpInstrDir = "build/instrserver";

	public FrHeapInstrumentConfig(String proxyClass) {
		this.proxyClass = proxyClass;
	}
}
