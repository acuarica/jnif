#ifndef JNIF_CODEATTRPARSER_HPP
#define JNIF_CODEATTRPARSER_HPP

#include "../base.hpp"

namespace jnif {

/**
 *
 */
template<typename TMethodVisitor>
class CodeParser {
public:
	TMethodVisitor& mv;
	ConstPool& cp;

	CodeParser(TMethodVisitor& mv, ConstPool& cp) :
			mv(mv), cp(cp) {
	}

	typedef void (*FrFuncPtr)(int offset, u1 opcode, BufferReader& br,
			CodeParser& code);

	void parse(BufferReader& br) {
		static FrFuncPtr OPVTABLE[256] = { /* nop */&FrParseZeroInstr,
		/*aconst_null*/&FrParseZeroInstr, &FrParseZeroInstr, // iconst_m1
				&FrParseZeroInstr, // iconst_0
				&FrParseZeroInstr, // iconst_1
				&FrParseZeroInstr, // iconst_2
				&FrParseZeroInstr, // iconst_3
				&FrParseZeroInstr, // iconst_4
				&FrParseZeroInstr, // iconst_5
				&FrParseZeroInstr, // lconst_0
				&FrParseZeroInstr, // lconst_1
				&FrParseZeroInstr, // fconst_0
				&FrParseZeroInstr, // fconst_1
				&FrParseZeroInstr, // fconst_2
				&FrParseZeroInstr, // dconst_0
				&FrParseZeroInstr, // dconst_1
				&_FrParseBiPushInstr, // bipush
				&_FrParseSiPushInstr, // sipush
				&FrParseLdcInstr, // ldc
				&FrParseLdcInstr, // ldc_w
				&FrParseLdcInstr, // ldc2_w
				&FrParseVarInstr, // iload
				&FrParseVarInstr, // lload
				&FrParseVarInstr, // fload
				&FrParseVarInstr, // dload
				&FrParseVarInstr, // aload
				&FrParseZeroInstr, // iload_0
				&FrParseZeroInstr, // iload_1
				&FrParseZeroInstr, // iload_2
				&FrParseZeroInstr, // iload_3
				&FrParseZeroInstr, // lload_0
				&FrParseZeroInstr, // lload_1
				&FrParseZeroInstr, // lload_2
				&FrParseZeroInstr, // lload_3
				&FrParseZeroInstr, // fload_0
				&FrParseZeroInstr, // fload_1
				&FrParseZeroInstr, // fload_2
				&FrParseZeroInstr, // fload_3
				&FrParseZeroInstr, // dload_0
				&FrParseZeroInstr, // dload_1
				&FrParseZeroInstr, // dload_2
				&FrParseZeroInstr, // dload_3
				&FrParseZeroInstr, // aload_0
				&FrParseZeroInstr, // aload_1
				&FrParseZeroInstr, // aload_2
				&FrParseZeroInstr, // aload_3
				&FrParseZeroInstr, // iaload
				&FrParseZeroInstr, // laload
				&FrParseZeroInstr, // faload
				&FrParseZeroInstr, // daload
				&FrParseZeroInstr, // aaload
				&FrParseZeroInstr, // baload
				&FrParseZeroInstr, // caload
				&FrParseZeroInstr, // saload
				&FrParseVarInstr, // istore
				&FrParseVarInstr, // lstore
				&FrParseVarInstr, // fstore
				&FrParseVarInstr, // dstore
				&FrParseVarInstr, // astore
				&FrParseZeroInstr, // istore_0
				&FrParseZeroInstr, // istore_1
				&FrParseZeroInstr, // istore_2
				&FrParseZeroInstr, // istore_3
				&FrParseZeroInstr, // lstore_0
				&FrParseZeroInstr, // lstore_1
				&FrParseZeroInstr, // lstore_2
				&FrParseZeroInstr, // lstore_3
				&FrParseZeroInstr, // fstore_0
				&FrParseZeroInstr, // fstore_1
				&FrParseZeroInstr, // fstore_2
				&FrParseZeroInstr, // fstore_3
				&FrParseZeroInstr, // dstore_0
				&FrParseZeroInstr, // dstore_1
				&FrParseZeroInstr, // dstore_2
				&FrParseZeroInstr, // dstore_3
				&FrParseZeroInstr, // astore_0
				&FrParseZeroInstr, // astore_1
				&FrParseZeroInstr, // astore_2
				&FrParseZeroInstr, // astore_3
				&FrParseZeroInstr, // iastore
				&FrParseZeroInstr, // lastore
				&FrParseZeroInstr, // fastore
				&FrParseZeroInstr, // dastore
				&FrParseZeroInstr, // aastore
				&FrParseZeroInstr, // bastore
				&FrParseZeroInstr, // castore
				&FrParseZeroInstr, // sastore
				&FrParseZeroInstr, // pop
				&FrParseZeroInstr, // pop2
				&FrParseZeroInstr, // dup
				&FrParseZeroInstr, // dup_x1
				&FrParseZeroInstr, // dup_x2
				&FrParseZeroInstr, // dup2
				&FrParseZeroInstr, // dup2_x1
				&FrParseZeroInstr, // dup2_x2
				&FrParseZeroInstr, // swap
				&FrParseZeroInstr, // iadd
				&FrParseZeroInstr, // ladd
				&FrParseZeroInstr, // fadd
				&FrParseZeroInstr, // dadd
				&FrParseZeroInstr, // isub
				&FrParseZeroInstr, // lsub
				&FrParseZeroInstr, // fsub
				&FrParseZeroInstr, // dsub
				&FrParseZeroInstr, // imul
				&FrParseZeroInstr, // lmul
				&FrParseZeroInstr, // fmul
				&FrParseZeroInstr, // dmul
				&FrParseZeroInstr, // idiv
				&FrParseZeroInstr, // ldiv
				&FrParseZeroInstr, // fdiv
				&FrParseZeroInstr, // ddiv
				&FrParseZeroInstr, // irem
				&FrParseZeroInstr, // lrem
				&FrParseZeroInstr, // frem
				&FrParseZeroInstr, // drem
				&FrParseZeroInstr, // ineg
				&FrParseZeroInstr, // lneg
				&FrParseZeroInstr, // fneg
				&FrParseZeroInstr, // dneg
				&FrParseZeroInstr, // ishl
				&FrParseZeroInstr, // lshl
				&FrParseZeroInstr, // ishr
				&FrParseZeroInstr, // lshr
				&FrParseZeroInstr, // iushr
				&FrParseZeroInstr, // lushr
				&FrParseZeroInstr, // iand
				&FrParseZeroInstr, // land
				&FrParseZeroInstr, // ior
				&FrParseZeroInstr, // lor
				&FrParseZeroInstr, // ixor
				&FrParseZeroInstr, // lxor
				&FrParseIincInstr, // iinc
				&FrParseZeroInstr, // i2l
				&FrParseZeroInstr, // i2f
				&FrParseZeroInstr, // i2d
				&FrParseZeroInstr, // l2i
				&FrParseZeroInstr, // l2f
				&FrParseZeroInstr, // l2d
				&FrParseZeroInstr, // f2i
				&FrParseZeroInstr, // f2l
				&FrParseZeroInstr, // f2d
				&FrParseZeroInstr, // d2i
				&FrParseZeroInstr, // d2l
				&FrParseZeroInstr, // d2f
				&FrParseZeroInstr, // i2b
				&FrParseZeroInstr, // i2c
				&FrParseZeroInstr, // i2s
				&FrParseZeroInstr, // lcmp
				&FrParseZeroInstr, // fcmpl
				&FrParseZeroInstr, // fcmpg
				&FrParseZeroInstr, // dcmpl
				&FrParseZeroInstr, // dcmpg
				&FrParseJumpInstr, // ifeq
				&FrParseJumpInstr, // ifne
				&FrParseJumpInstr, // iflt
				&FrParseJumpInstr, // ifge
				&FrParseJumpInstr, // ifgt
				&FrParseJumpInstr, // ifle
				&FrParseJumpInstr, // if_icmpeq
				&FrParseJumpInstr, // if_icmpne
				&FrParseJumpInstr, // if_icmplt
				&FrParseJumpInstr, // if_icmpge
				&FrParseJumpInstr, // if_icmpgt
				&FrParseJumpInstr, // if_icmple
				&FrParseJumpInstr, // if_acmpeq
				&FrParseJumpInstr, // if_acmpne
				&FrParseJumpInstr, // goto
				&FrParseJumpInstr, // jsr
				&FrParseVarInstr, // ret
				&_FrParseTableSwitchInstr, // tableswitch
				&_FrParseLookupSwitchInstr, // lookupswitch
				&FrParseZeroInstr, // ireturn
				&FrParseZeroInstr, // lreturn
				&FrParseZeroInstr, // freturn
				&FrParseZeroInstr, // dreturn
				&FrParseZeroInstr, // areturn
				&FrParseZeroInstr, // return
				&FrParseFieldInstr, // getstatic
				&FrParseFieldInstr, // putstatic
				&FrParseFieldInstr, // getfield
				&FrParseFieldInstr, // putfield
				&FrParseInvokeInstr, // invokevirtual
				&FrParseInvokeInstr, // invokespecial
				&FrParseInvokeInstr, // invokestatic
				&FrParseInvokeInterfaceInstr, // invokeinterface
				&FrParseInvokeDynamicInstr, // invokedynamic
				&FrParseTypeInstr, // new
				&_FrParseNewArrayInstr, // newarray
				&FrParseTypeInstr, // anewarray
				&FrParseZeroInstr, // arraylength
				&FrParseZeroInstr, // athrow
				&FrParseTypeInstr, // checkcast
				&FrParseTypeInstr, // instanceof
				&FrParseZeroInstr, // monitorenter
				&FrParseZeroInstr, // monitorexit
				&FrParseZeroInstr, // wide
				&FrParseMultiArrayInstr, // multianewarray
				&FrParseJumpInstr, // ifnull
				&FrParseJumpInstr, // ifnonnull
				&FrParse4__TODO__Instr, // goto_w
				&FrParse4__TODO__Instr, // jsr_w
				&FrParseReservedInstr, // breakpoint
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // RESERVED
				&FrParseReservedInstr, // impdep1
				&FrParseReservedInstr, // impdep2
				};

		while (!br.eor()) {
			int offset = br.offset();

			u1 opcode = br.readu1();

			FrFuncPtr opparser = OPVTABLE[opcode];
			opparser(offset, opcode, br, *this);
		}
	}

private:

	static void FrParseZeroInstr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {

		if (opcode == OPCODE_wide)
			ASSERT(false, "wide not supported yet");

		code.mv.visitZero(offset, opcode);
	}

	static void FrParseFieldInstr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {
		u2 fieldRefIndex = br.readu2();

		string className, name, desc;

		code.cp.getMemberRef(fieldRefIndex, &className, &name, &desc,
				CONSTANT_Fieldref);

		code.mv.visitField(offset, opcode, fieldRefIndex, className, name,
				desc);
	}

	static void _FrParseBiPushInstr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {
		u1 bytevalue = br.readu1();

		code.mv.visitBiPush(offset, opcode, bytevalue);
	}

	static void _FrParseSiPushInstr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {
		u2 shortvalue = br.readu2();

		code.mv.visitSiPush(offset, opcode, shortvalue);
	}

	static void _FrParseNewArrayInstr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {
		u1 atype = br.readu1();

		code.mv.visitNewArray(offset, opcode, atype);
	}

	static void FrParseTypeInstr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {
		u2 classIndex = br.readu2();

		string className = code.cp.getClazzName(classIndex);

		code.mv.visitType(offset, opcode, classIndex, className);
	}

	static void FrParseJumpInstr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {
		u2 targetOffset = br.readu2();

		code.mv.visitJump(offset, opcode, targetOffset);
	}

	static void FrParseMultiArrayInstr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {
		u2 classIndex = br.readu2();
		u1 dims = br.readu1();

		string className = code.cp.getClazzName(classIndex);

		code.mv.visitMultiArray(offset, opcode, classIndex, className, dims);
	}

	static void FrParseIincInstr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {
		u1 index = br.readu1();
		u1 value = br.readu1();

		code.mv.visitIinc(offset, opcode, index, value);
	}

	static void FrParseLdcInstr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {
		u2 arg;
		if (opcode == OPCODE_ldc) {
			u1 index = br.readu1();
			arg = index;
		} else {
			u2 index = br.readu2();
			arg = index;
		}

		code.mv.visitLdc(offset, opcode, arg);
	}

	static void FrParseInvokeDynamicInstr(int offset, u1 opcode,
			BufferReader& br, CodeParser& code) {
		EXCEPTION("FrParseInvokeDynamicInstr not implemented");
	}

	static void FrParseInvokeInterfaceInstr(int offset, u1 opcode,
			BufferReader& br, CodeParser& code) {
		u2 interMethodrefIndex = br.readu2();
		u1 count = br.readu1();
		u1 zero = br.readu1();

		ASSERT(count != 0, "count must be non-zero");
		ASSERT(zero == 0, "Fourth operand must be zero");

		string className, name, desc;
		code.cp.getMemberRef(interMethodrefIndex, &className, &name, &desc,
				CONSTANT_InterfaceMethodref);

		code.mv.visitInvokeInterface(offset, opcode, interMethodrefIndex,
				className, name, desc, count);
	}

	static void FrParseInvokeInstr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {
		u2 methodrefIndex = br.readu2();

		string className, name, desc;
		code.cp.getMemberRef(methodrefIndex, &className, &name, &desc,
				CONSTANT_Methodref);

		code.mv.visitInvoke(offset, opcode, methodrefIndex, className, name,
				desc);
	}

	static void FrParseVarInstr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {
		u1 lvindex = br.readu1();

		code.mv.visitVar(offset, opcode, lvindex);
	}

	static void _FrParseTableSwitchInstr(int offset, u1 opcode,
			BufferReader& br, CodeParser& code) {
		for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
			u1 pad = br.readu1();
			ASSERT(pad == 0, "Padding must be zero");
		}

		bool check = br.offset() % 4 == 0;
		ASSERT(check, "%d", br.offset());

		int def = br.readu4();
		int low = br.readu4();
		int high = br.readu4();

		ASSERT(low <= high, "low (%d) must be less or equal than high (%d)",
				low, high);

		vector < u4 > targets;
		for (int i = 0; i < high - low + 1; i++) {
			u4 targetOffset = br.readu4();
			targets.push_back(targetOffset);
		}

		code.mv.visitTableSwitch(offset, opcode, def, low, high, targets);
	}

	static void _FrParseLookupSwitchInstr(int offset, u1 opcode,
			BufferReader& br, CodeParser& code) {
		for (int i = 0; i < (((-offset - 1) % 4) + 4) % 4; i++) {
			u1 pad = br.readu1();
			ASSERT(pad == 0, "Padding must be zero");
		}

		u4 defbyte = br.readu4();
		u4 npairs = br.readu4();

		vector < u4 > keys;
		vector < u4 > targets;
		for (u4 i = 0; i < npairs; i++) {
			u4 key = br.readu4();
			u4 offsetTarget = br.readu4();

			keys.push_back(key);
			targets.push_back(offsetTarget);
		}

		code.mv.visitLookupSwitch(offset, opcode, defbyte, npairs, keys,
				targets);
	}

	static void FrParse4__TODO__Instr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {
		EXCEPTION("FrParse4__TODO__Instr not implemented");
	}

	static void FrParseReservedInstr(int offset, u1 opcode, BufferReader& br,
			CodeParser& code) {
		EXCEPTION("FrParseReservedInstr not implemented");
	}
};

template<typename ...TAttrParserList>
class CodeAttrParser {
public:

	static constexpr const char* AttrName = "Code";

	template<typename TCodeVisitor>
	static void parse(BufferReader& br, TCodeVisitor& mmv, ConstPool& cp,
			u2 nameIndex) {
		auto bv = mmv.visitCode(nameIndex);

		u2 maxStack = br.readu2();
		u2 maxLocals = br.readu2();

		bv.enter(maxStack, maxLocals);

		u4 codeLen = br.readu4();
		const u1* codeBuf = br.pos();
		br.skip(codeLen);

		{
			BufferReader br(codeBuf, codeLen);
			CodeParser<decltype(bv)>(bv, cp).parse(br);
		}

		u2 exceptionTableCount = br.readu2();
		for (int i = 0; i < exceptionTableCount; i++) {
			u2 startpc = br.readu2();
			u2 endpc = br.readu2();
			u2 handlerpc = br.readu2();
			u2 catchtype = br.readu2();

			bv.visitExceptionEntry(startpc, endpc, handlerpc, catchtype);
		}

		AttrsParser<TAttrParserList...>::parse(br, cp, bv);

		bv.exit();
	}
};

}

#endif
