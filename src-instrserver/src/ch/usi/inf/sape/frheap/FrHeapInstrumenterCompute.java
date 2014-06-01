package ch.usi.inf.sape.frheap;

import java.io.IOException;

import org.objectweb.asm.ClassReader;
import org.objectweb.asm.ClassWriter;

/**
 * 
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-6.html
 * 
 * @author luigi
 * 
 */
public class FrHeapInstrumenterCompute extends FrHeapInstrumenter {

	@Override
	public byte[] instrumentClass(byte[] classBytes, String className)
			throws IOException {

		ClassWriter cw = new ClassWriter(ClassWriter.COMPUTE_FRAMES);
		ClassReader cr = new ClassReader(classBytes);

		cr.accept(cw, 0);

		byte[] instrClassBytes = cw.toByteArray();

		return instrClassBytes;
	}
}
