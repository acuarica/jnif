package ch.usi.inf.sape.frheap;

import java.io.IOException;

import org.objectweb.asm.ClassReader;
import org.objectweb.asm.ClassVisitor;
import org.objectweb.asm.ClassWriter;
import org.objectweb.asm.MethodVisitor;
import org.objectweb.asm.Opcodes;

/**
 * 
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-6.html
 * 
 * @author luigi
 * 
 */
public class FrHeapInstrumenterAll extends FrHeapInstrumenter {

	@Override
	public byte[] instrumentClass(byte[] classBytes, String className)
			throws IOException {

		ClassWriter cw = new ClassWriter(ClassWriter.COMPUTE_MAXS
				| ClassWriter.COMPUTE_FRAMES);

		ClassReader cr = new ClassReader(classBytes);
		ClassTransformer ct = new ClassTransformer(cw, className);

		cr.accept(ct, 0);

		byte[] instrClassBytes = cw.toByteArray();

		return instrClassBytes;
	}

	private class ClassTransformer extends ClassVisitor {

		private String _className;

		public ClassTransformer(ClassVisitor cv, String className) {
			super(Opcodes.ASM4, cv);
			_className = className;
		}

		public MethodVisitor visitMethod(int access, String name, String desc,
				String signature, String[] exceptions) {
			MethodVisitor mv = cv.visitMethod(access, name, desc, signature,
					exceptions);

			if (mv != null) {
				if (!skipCompute(_className)) {
					mv = new MethodTransformer(mv);
				}
			}

			return mv;
		}
	}

	/**
	 * 
	 * @author luigi
	 * 
	 */
	private class MethodTransformer extends MethodVisitor {

		public MethodTransformer(MethodVisitor mv) {
			super(Opcodes.ASM4, mv);
		}

		private void instrOpcode(int opcode) {
			// mv.visitIntInsn(Opcodes.BIPUSH, opcode);
			// mv.visitMethodInsn(Opcodes.INVOKESTATIC, config.proxyClass,
			// "opcode", "(I)V");
			mv.visitInsn(Opcodes.NOP);
		}

		@Override
		public void visitInsn(int opcode) {
			instrOpcode(opcode);
			mv.visitInsn(opcode);
		}

		@Override
		public void visitIntInsn(int opcode, int operand) {
			instrOpcode(opcode);
			mv.visitIntInsn(opcode, operand);
		}

		@Override
		public void visitTypeInsn(int opcode, String type) {
			instrOpcode(opcode);
			mv.visitTypeInsn(opcode, type);
		}

		@Override
		public void visitFieldInsn(int opcode, String owner, String name,
				String desc) {
			instrOpcode(opcode);
			mv.visitFieldInsn(opcode, owner, name, desc);
		}
	}
}
