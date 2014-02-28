#ifndef JNIF_CLASSDEFAULTVISITOR_HPP
#define JNIF_CLASSDEFAULTVISITOR_HPP

#include "base.hpp"

namespace jnif {

/**
 * Represents the model for visiting class files.
 * The default implementation is empty, i.e.,
 * for every method it does nothing.
 * This class is a singleton class, the only available instance is
 * inst.
 */
class ClassDefaultVisitor {
public:

	class Field {
	public:
		inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
		}
	private:
		friend class ClassDefaultVisitor;
		inline Field() {
		}
	};

	class Method {
	public:

		class Code {
		public:
			inline void enter(u2 maxStack, u2 maxLocals) {
			}

			inline void exit() {
			}

			inline void visitZero(int offset, u1 opcode) {
			}

			inline void visitField(int offset, u1 opcode, u2 fieldRefIndex,
					const string& className, const string& name,
					const string& desc) {
			}

			inline void visitBiPush(int offset, u1 opcode, u1 bytevalue) {
			}

			inline void visitSiPush(int offset, u1 opcode, u2 shortvalue) {
			}

			inline void visitNewArray(int offset, u1 opcode, u1 atype) {
			}

			inline void visitType(int offset, u1 opcode, u2 classIndex,
					const string& className) {
			}

			inline void visitJump(int offset, u1 opcode, u2 targetOffset) {
			}

			inline void visitMultiArray(int offset, u1 opcode, u2 classIndex,
					const string& className, u4 dims) {
			}

			inline void visitIinc(int offset, u1 opcode, u1 index, u1 value) {
			}

			inline void visitLdc(int offset, u1 opcode, u2 arg) {
			}

			inline void visitInvokeInterface(int offset, u1 opcode,
					u2 interMethodrefIndex, const string& className,
					const string& name, const string& desc, u1 count) {
			}

			inline void visitInvoke(int offset, u1 opcode, u2 methodrefIndex,
					const string& className, const string& name,
					const string& desc) {
			}

			inline void visitVar(int offset, u1 opcode, u2 lvindex) {
			}

			inline void visitTableSwitch(int offset, u1 opcode, int def,
					int low, int high, const vector<u4>& targets) {
			}

			inline void visitLookupSwitch(int offset, u1 opcode, u4 defbyte,
					u4 npairs, const vector<u4>& keys,
					const vector<u4>& targets) {
			}

			inline void visitExceptionEntry(u2 startpc, u2 endpc, u2 handlerpc,
					u2 catchtype) {
			}

			inline void visitLnt(u2 nameIndex, u2 startpc, u2 lineno) {
			}

			inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			}

		private:
			friend class Method;
			inline Code() {
			}
		};

		inline Code visitCode(u2 nameIndex) {
			return Code();
		}

		inline void visitException(u2 exceptionIndex, vector<u2>& es) {
		}

		inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
		}
	private:
		friend class ClassDefaultVisitor;
		inline Method() {
		}
	};

	inline void visitVersion(Magic magic, u2 minor, u2 major) {
	}

	inline void visitConstPool(const ConstPool& cp) {
	}

	inline void visitThis(u2 accessFlags, u2 thisClassIndex,
			u2 superClassIndex) {
	}

	inline void visitInterface(u2 interIndex) {
	}

	inline Field visitField(u2 accessFlags, u2 nameIndex, u2 descIndex) {
		return Field();
	}

	inline Method visitMethod(u2 accessFlags, u2 nameIndex, u2 descIndex) {
		return Method();
	}

	inline void visitSourceFile(u2 nameIndex, u2 sourceFileIndex) {
	}

	inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
	}

	/**
	 * The singleton instance.
	 */
	static ClassDefaultVisitor inst;

private:
	inline ClassDefaultVisitor() {
	}
};

ClassDefaultVisitor ClassDefaultVisitor::inst;

}

#endif
