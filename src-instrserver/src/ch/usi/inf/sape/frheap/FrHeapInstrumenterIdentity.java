package ch.usi.inf.sape.frheap;

import java.io.IOException;
import java.io.InputStream;

import org.objectweb.asm.ClassReader;
import org.objectweb.asm.ClassWriter;

public class FrHeapInstrumenterIdentity extends FrHeapInstrumenter {

	@Override
	public byte[] instrumentClass(InputStream classBytes, String className)
			throws IOException {

		ClassWriter cw = new ClassWriter(0);
		ClassReader cr = new ClassReader(classBytes);
		cr.accept(cw, 0);

		byte[] instrClassBytes = cw.toByteArray();

		return instrClassBytes;
	}

}
