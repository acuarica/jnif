package ch.usi.inf.sape.frheap;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import org.apache.log4j.Logger;
import org.objectweb.asm.ClassReader;
import org.objectweb.asm.ClassVisitor;
import org.objectweb.asm.ClassWriter;
import org.objectweb.asm.FieldVisitor;
import org.objectweb.asm.MethodVisitor;
import org.objectweb.asm.Opcodes;

/**
 * 
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-6.html
 * 
 * @author luigi
 * 
 */
public class FrHeapInstrument {

	private final static Logger logger = Logger.getLogger(FrHeapInstrument.class);

	private final FrHeapInstrumentConfig _config;

	public FrHeapInstrument(FrHeapInstrumentConfig config) {
		_config = config;
	}

	public byte[] instrumentClass(InputStream classBytes, String className) throws IOException {
		ClassWriter cw = new ClassWriter(ClassWriter.COMPUTE_MAXS | ClassWriter.COMPUTE_FRAMES);

		ClassReader cr = new ClassReader(classBytes);
		ClassTransformer ct = new ClassTransformer(cw, className);
		cr.accept(ct, 0);

		byte[] instrClassBytes = cw.toByteArray();
		dumpInstrumentedClassFile(instrClassBytes, className);

		return instrClassBytes;
	}

	private void dumpInstrumentedClassFile(byte[] instrClassBytes, String className) {
		String fileName = _config.dumpInstrDir + "/" + className.replace('.', '/') + ".class";

		String dir = new File(fileName).getParent();

		logger.trace("Creating dir " + dir);
		new File(dir).mkdirs();

		logger.trace(String.format("Dumping class file for class %s in %s", className, fileName));

		try {
			FileOutputStream output = new FileOutputStream(fileName);
			output.write(instrClassBytes);
			output.close();
		} catch (IOException e) {
			logger.warn("Unable to write instrumented class file in " + fileName, e);
		}
	}

	/**
	 * 
	 * @author luigi
	 * 
	 */
	private class ClassTransformer extends ClassVisitor {

		private String _className;

		public ClassTransformer(ClassVisitor cv, String className) {
			super(Opcodes.ASM4, cv);

			_className = className;
		}

		@Override
		public void visit(int version, int access, String name, String signature, String superName, String[] interfaces) {
			super.visit(version, access, name, signature, superName, interfaces);

			if (_className.equals("java/lang/Object")) {
				logger.debug("Adding field to object class");

				visitField(Opcodes.ACC_FINAL | Opcodes.ACC_PRIVATE, "__elcampito__", "J", null, 28L);
			}
		}

		@Override
		public FieldVisitor visitField(int access, String name, String desc, String signature, Object value) {
			// logger.debug(String.format("%d %s %s %s %s", access, name, desc,
			// signature, value));

			return super.visitField(access, name, desc, signature, value);
		}

		public MethodVisitor visitMethod(int access, String name, String desc, String signature, String[] exceptions) {
			MethodVisitor mv = cv.visitMethod(access, name, desc, signature, exceptions);

			if (mv != null) {
				if (isMainMethod(access, name, desc)) {
					mv = new MainMethodTransformer(mv);
				}

				if (_className.equals("java/lang/Object") && name.equals("<init>") && desc.equals("()V")) {
					mv = new ObjectInitMethodTransformer(mv);
				}

				mv = new MethodTransformer(mv);
			}

			return mv;
		}

		private boolean isMainMethod(int access, String name, String desc) {
			return (access & Opcodes.ACC_STATIC) != 0 && (access & Opcodes.ACC_PUBLIC) != 0 && name.equals("main")
					&& desc.equals("([Ljava/lang/String;)V");
		}
	}

	/**
	 * 
	 * @author luigi
	 * 
	 */
	private class ObjectInitMethodTransformer extends MethodVisitor {

		public ObjectInitMethodTransformer(MethodVisitor mv) {
			super(Opcodes.ASM4, mv);
		}

		@Override
		public void visitCode() {
			mv.visitCode();

			// Loads this object
			// mv.visitVarInsn(Opcodes.ALOAD, 0);

			// mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass,
			// "alloc", "(Ljava/lang/Object;)V");
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

		// @Override
		public void visitIntInsn32(int opcode, int operand) {
			if (opcode == Opcodes.NEWARRAY) {
				// FORMAT: newarray atype
				// OPERAND STACK: ... | count: int -> ... | arrayref

				// STACK: ... | count

				mv.visitInsn(Opcodes.DUP);
				// STACK: ... | count | count

				mv.visitIntInsn(opcode, operand); // newarray
				// STACK: ... | count | arrayref

				mv.visitInsn(Opcodes.DUP_X1);
				// STACK: ... | arrayref | count | arrayref

				mv.visitLdcInsn(operand);
				// STACK: ... | arrayref | count | arrayref | atype

				mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass, "newArrayEvent", "(ILjava/lang/Object;I)V");
				// STACK: ... | arrayref

			} else {
				mv.visitIntInsn(opcode, operand);
			}
		}

		// @Override
		public void visitTypeInsn32(int opcode, String type) {
			if (opcode == Opcodes.ANEWARRAY) {
				// FORMAT: anewarray (indexbyte1 << 8) | indexbyte2
				// OPERAND STACK: ... | count: int -> ... | arrayref

				// STACK: ... | count

				mv.visitInsn(Opcodes.DUP);
				// STACK: ... | count | count

				mv.visitTypeInsn(opcode, type); // anewarray
				// STACK: ... | count | arrayref

				mv.visitInsn(Opcodes.DUP_X1);
				// STACK: ... | arrayref | count | arrayref

				mv.visitLdcInsn(type);
				// STACK: ... | arrayref | count | arrayref | type

				mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass, "aNewArrayEvent",
						"(ILjava/lang/Object;Ljava/lang/String;)V");
				// STACK: ... | arrayref
			} else {
				mv.visitTypeInsn(opcode, type);
			}
		}

		// @Override
		public void visitMultiANewArrayInsn32(String desc, int dims) {
			// FORMAT: multianewarray | indexbyte1 | indexbyte2 | dimensions
			// OPERAND STACK: ... | count1 | [ count2 | ...]] -> ... | arrayref

			// STACK: ... | count1 | [ count2 | [ count3 | ... ]]

			if (dims == 1) {
				mv.visitInsn(Opcodes.DUP);
				// STACK: ... | count1 | count1

			} else if (dims == 2) {
				mv.visitInsn(Opcodes.DUP2);
				// STACK: ... | count1 | count2 | count1 | count2

			} else {
				// STACK: ... | count1 | count2 | count3 | [ count4 | ...]
			}

			mv.visitMultiANewArrayInsn(desc, dims);

			if (dims == 1) {
				// STACK: ... | count1 | arrayref

				mv.visitInsn(Opcodes.DUP_X1);
				// STACK: ... | arrayref | count1 | arrayref

				mv.visitLdcInsn(desc);
				// STACK: ... | arrayref | count1 | arrayref | desc

				mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass, "multiANewArray1Event",
						"(ILjava/lang/Object;Ljava/lang/String;)V");
				// STACK: ... | arrayref

			} else if (dims == 2) {
				// STACK: ... | count1 | count2 | arrayref

				mv.visitInsn(Opcodes.DUP_X2);
				// STACK: ... | arrayref | count1 | count2 | arrayref

				mv.visitLdcInsn(desc);
				// STACK: ... | arrayref | count1 | count2 | arrayref | desc

				mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass, "multiANewArray2Event",
						"(IILjava/lang/Object;Ljava/lang/String;)V");

				// STACK: ... | arrayref

			} else {
				// STACK: ... | arrayref

				mv.visitInsn(Opcodes.DUP);
				// STACK: ... | arrayref | arrayref

				mv.visitLdcInsn(dims);
				// STACK: ... | arrayref | arrayref | dims

				mv.visitLdcInsn(desc);
				// STACK: ... | arrayref | arrayref | dims | desc

				mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass, "multiANewArrayNEvent",
						"(Ljava/lang/Object;ILjava/lang/String;)V");
				// STACK: ... | arrayref

			}
		}

		private boolean isObjectOrArray(String desc) {
			return desc.charAt(0) == 'L' || desc.charAt(0) == '[';
		}

		@Override
		public void visitFieldInsn(int opcode, String owner, String name, String desc) {
			if (opcode == Opcodes.PUTFIELD && isObjectOrArray(desc)) {
				// FORMAT: putfield (indexbyte1 << 8) | indexbyte2
				// OPERAND STACK: ... | objectref | value -> ...

				// STACK: ... | objectref | value

				mv.visitInsn(Opcodes.DUP2);
				// STACK: ... | objectref | value | objectref | value

				mv.visitFieldInsn(opcode, owner, name, desc); // putfield
				// STACK: ... | objectref | value

				mv.visitLdcInsn(name);
				// STACK: ... | objectref | value | name

				mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass, "putFieldEvent",
						"(Ljava/lang/Object;Ljava/lang/Object;Ljava/lang/String;)V");
				// STACK: ...

			} else if (opcode == Opcodes.PUTSTATIC && isObjectOrArray(desc)) {
				// FORMAT: putstatic (indexbyte1 << 8) | indexbyte2
				// OPERAND STACK: ... | value -> ...

				// STACK: ... | value

				mv.visitInsn(Opcodes.DUP);
				// STACK: ... | value | value

				mv.visitFieldInsn(opcode, owner, name, desc); // putstatic
				// STACK: ... | value

				mv.visitLdcInsn(owner);
				// STACK: ... | value | owner

				mv.visitLdcInsn(name);
				// STACK: ... | value | owner | name

				mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass, "putStaticEvent",
						"(Ljava/lang/Object;Ljava/lang/String;Ljava/lang/String;)V");
				// STACK: ...

			} else {
				mv.visitFieldInsn(opcode, owner, name, desc);
			}
		}

		@Override
		public void visitInsn(int opcode) {
			if (opcode == Opcodes.AASTORE) {
				// FORMAT: aastore
				// OPERAND STACK: ... | arrayref | index | value -> ...

				// STACK: ... | arrayref | index | value

				mv.visitInsn(Opcodes.DUP2_X1);
				// STACK: ... | index | value | arrayref | index | value

				mv.visitInsn(Opcodes.DUP2_X1);
				// STACK: ... | index | value | index | value | arrayref | index
				// | value

				mv.visitInsn(Opcodes.POP2);
				// STACK: ... | index | value | index | value | arrayref

				mv.visitInsn(Opcodes.DUP_X2);
				// STACK: ... | index | value | arrayref | index | value |
				// arrayref

				mv.visitInsn(Opcodes.DUP_X2);
				// STACK: ... | index | value | arrayref | arrayref | index |
				// value | arrayref

				mv.visitInsn(Opcodes.POP);
				// STACK: ... | index | value | arrayref | arrayref | index |
				// value

				mv.visitInsn(opcode); // aastore
				// STACK: ... | index | value | arrayref

				mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass, "aastoreEvent",
						"(ILjava/lang/Object;Ljava/lang/Object;)V");

				// STACK: ... |

			} else {
				mv.visitInsn(opcode);
			}
		}
	}

	/**
	 * 
	 * @author luigi
	 * 
	 */
	private class MainMethodTransformer extends MethodVisitor {

		public MainMethodTransformer(MethodVisitor mv) {
			super(Opcodes.ASM4, mv);
		}

		@Override
		public void visitCode() {
			mv.visitCode();
			mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass, "enterMainMethod", "()V");
		}

		@Override
		public void visitInsn(int opcode) {
			if ((opcode >= Opcodes.IRETURN && opcode <= Opcodes.RETURN) || opcode == Opcodes.ATHROW) {
				mv.visitMethodInsn(Opcodes.INVOKESTATIC, _config.proxyClass, "exitMainMethod", "()V");
			}

			mv.visitInsn(opcode);
		}
	}
}
