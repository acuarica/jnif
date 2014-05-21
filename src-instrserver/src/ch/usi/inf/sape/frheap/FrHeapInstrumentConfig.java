package ch.usi.inf.sape.frheap;

public class FrHeapInstrumentConfig {

	String proxyClass;

	String dumpInstrDir = "build/instrserverdump";

	public FrHeapInstrumentConfig(String proxyClass) {
		this.proxyClass = proxyClass;
	}
}
