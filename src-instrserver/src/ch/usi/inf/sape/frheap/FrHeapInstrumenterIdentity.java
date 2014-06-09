package ch.usi.inf.sape.frheap;

import java.io.IOException;

import org.objectweb.asm.ClassReader;
import org.objectweb.asm.ClassWriter;

public class FrHeapInstrumenterIdentity extends FrHeapInstrumenter {

	@Override
	public byte[] instrumentClass(byte[] classBytes, String className)
			throws IOException {

		ClassReader cr = new ClassReader(classBytes);

		ClassWriter cw = new ClassWriter(0);
		cr.accept(cw, 0);

		byte[] instrClassBytes = cw.toByteArray();

		return instrClassBytes;
	}

}
