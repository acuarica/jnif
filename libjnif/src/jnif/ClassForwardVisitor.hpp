#ifndef __BCI_CLASS_FORWARD_VISITOR_H__
#define	__BCI_CLASS_FORWARD_VISITOR_H__

#include "base.hpp"
#include "tree/ConstPool.hpp"

namespace JNIFNS {

/**
 *
 */
template<typename TVisitor>
class ClassForwardVisitor {
public:

	class Field {
	public:

		Field(typename TVisitor::Field& fv) :
				fv(fv) {
		}

		inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			fv.visitAttr(nameIndex, len, data);
		}
	private:
		typename TVisitor::Field fv;
	};

	class Method {
	public:

		class Code {
		public:
			typename TVisitor::Method::Code bv;
			inline Code(typename TVisitor::Method::Code& bv) :
					bv(bv) {
			}

			inline void enter(u2 maxStack, u2 maxLocals) {
				bv.enter(maxStack, maxLocals);
			}

			inline void exit() {
				bv.exit();
			}

			inline void visitZero(int offset, u1 opcode) {
				bv.visitZero(offset, opcode);
			}

			inline void visitField(int offset, u1 opcode, u2 fieldRefIndex,
					const string& className, const string& name,
					const string& desc) {
				bv.visitField(offset, opcode, fieldRefIndex, className, name,
						desc);
			}

			inline void visitBiPush(int offset, u1 opcode, u1 bytevalue) {
				bv.visitBiPush(offset, opcode, bytevalue);
			}

			inline void visitSiPush(int offset, u1 opcode, u2 shortvalue) {
				bv.visitSiPush(offset, opcode, shortvalue);
			}

			inline void visitNewArray(int offset, u1 opcode, u1 atype) {
				bv.visitNewArray(offset, opcode, atype);
			}

			inline void visitType(int offset, u1 opcode, u2 classIndex,
					const string& className) {
				bv.visitType(offset, opcode, classIndex, className);
			}

			inline void visitJump(int offset, u1 opcode, u2 targetOffset) {
				bv.visitJump(offset, opcode, targetOffset);
			}

			inline void visitMultiArray(int offset, u1 opcode, u2 classIndex,
					const string& className, u4 dims) {
				bv.visitMultiArray(offset, opcode, classIndex, className, dims);
			}

			inline void visitIinc(int offset, u1 opcode, u1 index, u1 value) {
				bv.visitIinc(offset, opcode, index, value);
			}

			inline void visitLdc(int offset, u1 opcode, u2 arg) {
				bv.visitLdc(offset, opcode, arg);
			}

			inline void visitInvokeInterface(int offset, u1 opcode,
					u2 interMethodrefIndex, const string& className,
					const string& name, const string& desc, u1 count) {
				bv.visitInvokeInterface(offset, opcode, interMethodrefIndex,
						className, name, desc, count);
			}

			inline void visitInvoke(int offset, u1 opcode, u2 methodrefIndex,
					const string& className, const string& name,
					const string& desc) {
				bv.visitInvoke(offset, opcode, methodrefIndex, className, name,
						desc);
			}

			inline void visitVar(int offset, u1 opcode, u2 lvindex) {
				bv.visitVar(offset, opcode, lvindex);
			}

			inline void visitTableSwitch(int offset, u1 opcode, int def,
					int low, int high, const vector<u4>& targets) {
				bv.visitTableSwitch(offset, opcode, def, low, high, targets);
			}

			inline void visitLookupSwitch(int offset, u1 opcode, u4 defbyte,
					u4 npairs, const vector<u4>& keys,
					const vector<u4>& targets) {
				bv.visitLookupSwitch(offset, opcode, defbyte, npairs, keys,
						targets);
			}

			inline void visitExceptionEntry(u2 startpc, u2 endpc, u2 handlerpc,
					u2 catchtype) {
				bv.visitExceptionEntry(startpc, endpc, handlerpc, catchtype);
			}

			inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
				bv.visitAttr(nameIndex, len, data);
			}
		private:
		};

		Method(typename TVisitor::Method& mv) :
				mv(mv) {
		}

		inline Code visitCode(u2 nameIndex) {
			typename TVisitor::Method::Code bv = mv.visitCode(nameIndex);
			return Code(bv);
		}

		inline void visitException(u2 exceptionIndex, vector<u2>& es) {
			mv.visitException(exceptionIndex, es);
		}

		inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
			mv.visitAttr(nameIndex, len, data);
		}
		typename TVisitor::Method mv;
	private:
	};

	inline void visitVersion(u4 magic, u2 minor, u2 major) {
		cv.visitVersion(magic, minor, major);
	}

	inline void visitConstPool(ConstPool& cp) {
		cv.visitConstPool(cp);
	}

	inline void visitThis(u2 accessFlags, u2 thisClassIndex,
			u2 superClassIndex) {
		cv.visitThis(accessFlags, thisClassIndex, superClassIndex);
	}

	inline void visitInterface(u2 interIndex) {
		cv.visitInterface(interIndex);
	}

	inline Field visitField(u2 accessFlags, u2 nameIndex, u2 descIndex) {
		auto fv = cv.visitField(accessFlags, nameIndex, descIndex);
		return Field(fv);
	}

	inline Method visitMethod(u2 accessFlags, u2 nameIndex, u2 descIndex) {
		auto mv = cv.visitMethod(accessFlags, nameIndex, descIndex);
		return Method(mv);
	}

	inline void visitSourceFile(u2 nameIndex, u2 sourceFileIndex) {
		cv.visitSourceFile(nameIndex, sourceFileIndex);
	}

	inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
		cv.visitAttr(nameIndex, len, data);
	}

protected:
	inline ClassForwardVisitor(TVisitor& cv) :
			cv(cv) {
	}

	TVisitor& cv;
};

}

#endif
