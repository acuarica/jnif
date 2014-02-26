#ifndef __BCI__INSTR_VISITOR__HPP__
#define	__BCI__INSTR_VISITOR__HPP__

#include "base.hpp"
#include "ClassForwardVisitor.hpp"

using namespace bci;

template<typename TVisitor>
class InstrVisitor: public ClassForwardVisitor<TVisitor> {
	typedef ClassForwardVisitor<TVisitor> base;
	using base::cv;
public:

	class Method: public base::Method {
		using base::Method::mv;
	public:

		InstrVisitor& instr;

		Method(typename TVisitor::Method& mv, InstrVisitor& instr) :
				base::Method(mv), instr(instr) {
		}

		class Code: public base::Method::Code {
			using base::Method::Code::bv;
		public:
			InstrVisitor& instr;
			Code(typename TVisitor::Method::Code& bv, InstrVisitor& instr) :
					base::Method::Code(bv), instr(instr) {
			}

			inline void enter(u2 maxStack, u2 maxLocals) {
				bv.enter(maxStack, maxLocals);

				const string& methodName = instr.cp->getUtf8(instr.nameIndex);

				if (methodName == "<init>321") {
					bv.visitZero(-42, OPCODE_nop);

					bv.visitZero(-87, OPCODE_aload_0);

					bv.visitInvoke(-32, OPCODE_invokestatic,
							instr.methodRefIndex, "hola", "q", "tal");
				}
			}

			inline void visitNewArray(int offset, u1 opcode, u1 atype) {
				// FORMAT: newarray atype
				// OPERAND STACK: ... | count: int -> ... | arrayref

				// STACK: ... | count

				bv.visitZero(offset, OPCODE_dup);
				// STACK: ... | count | count

				bv.visitNewArray(offset, opcode, atype); // newarray
				// STACK: ... | count | arrayref

				bv.visitZero(offset, OPCODE_dup_x1);
				// STACK: ... | arrayref | count | arrayref

				bv.visitBiPush(offset, OPCODE_bipush, atype);
				//u2 typeindex = instr.cp->addInteger(atype);

				//bv.visitLdc(offset, OPCODE_ldc_w, typeindex);
				// STACK: ... | arrayref | count | arrayref | atype

				bv.visitInvoke(offset, OPCODE_invokestatic,
						instr.methodRefIndex2, "", "", "");
				// STACK: ... | arrayref

			}

			inline void visitAttr(u2 nameIndex, u4 len, const u1* data) {
				const string& attrName = instr.cp->getUtf8(nameIndex);
				if (attrName != "StackMapTable2345") {
					bv.visitAttr(nameIndex, len, data);
				}
			}
		};

		Code visitCode(u2 nameIndex) {
			auto bv = mv.visitCode(nameIndex);
			return Code(bv, instr);

		}
	};

	InstrVisitor(TVisitor& cv) :
			ClassForwardVisitor<TVisitor>(cv) {
	}

	inline void visitConstPool(ConstPool& cp) {
		u2 classIndex = cp.addClass("frproxy/FrInstrProxy");

		methodRefIndex = cp.addMethodRef(classIndex, "alloc",
				"(Ljava/lang/Object;)V");

		methodRefIndex2 = cp.addMethodRef(classIndex, "newArrayEvent",
				"(ILjava/lang/Object;I)V");

		//this->methodRefIndex = methodRefIndex;
		//this->methodRefIndex2 = methodRefIndex2;

		this->cp = &cp;

		base::visitConstPool(cp);
	}

	inline Method visitMethod(u2 accessFlags, u2 nameIndex, u2 descIndex) {

		this->nameIndex = nameIndex;
		typename TVisitor::Method mv = base::cv.visitMethod(accessFlags,
				nameIndex, descIndex);

		return Method(mv, *this);
	}

	ConstPool* cp;
	u2 nameIndex;
	u2 methodRefIndex;
	u2 methodRefIndex2;
};

#endif
