#ifndef JNIF_CODEATTR_HPP
#define JNIF_CODEATTR_HPP

#include <sstream>

#include "../base.hpp"

namespace jnif {

#define ITEM_Top 0
#define ITEM_Integer 1
#define ITEM_Float 2
#define ITEM_Long 4
#define ITEM_Double 3
#define ITEM_Null 5
#define ITEM_UninitializedThis 6
#define ITEM_Object 7
#define ITEM_Uninitialized 8

typedef u1 Opcode;

enum OpKind {
	KIND_ZERO,
	KIND_BIPUSH,
	KIND_SIPUSH,
	KIND_LDC,
	KIND_VAR,
	KIND_IINC,
	KIND_JUMP,
	KIND_TABLESWITCH,
	KIND_LOOKUPSWITCH,
	KIND_FIELD,
	KIND_INVOKE,
	KIND_INVOKEINTERFACE,
	KIND_INVOKEDYNAMIC,
	KIND_TYPE,
	KIND_NEWARRAY,
	KIND_MULTIARRAY,
	KIND_PARSE4TODO,
	KIND_RESERVED
};

typedef u2 ClassIndex;

typedef u2 Label;

struct Inst {
	Opcode opcode;
	OpKind kind;

	union {
		struct {
			int value;
		} push;
		struct {
			u2 valueIndex;
		} ldc;
		struct {
			u1 lvindex;
		} var;
		struct {
			u1 index;
			u1 value;
		} iinc;
		struct {
			Label label;
		} jump;
		struct {
			u2 fieldRefIndex;
		} field;
		struct {
			u2 methodRefIndex;
		} invoke;
		struct {
			u2 interMethodRefIndex;
			u1 count;
		} invokeinterface;
		struct {
			ClassIndex classIndex;
		} type;
		struct {
			u1 atype;
		} newarray;
		struct {
			ClassIndex classIndex;
			u1 dims;
		} multiarray;
	};

	struct {
		int def;
		int low;
		int high;
		std::vector<u4> targets;
	} ts;

	struct {
		u4 defbyte;
		u4 npairs;
		std::vector<u4> keys;
		std::vector<u4> targets;
	} ls;
};

typedef std::vector<Inst> InstList;

/**
 * Represent the Code attribute of a method.
 */
struct CodeAttr: Attr {

	struct ExceptionEntry {
		u2 startpc;
		u2 endpc;
		u2 handlerpc;
		u2 catchtype;
	};

	CodeAttr(u2 nameIndex) :
			Attr(nameIndex), maxStack(0), maxLocals(0) {
	}

	virtual ~CodeAttr() {
	}

	u2 maxStack;
	u2 maxLocals;

	InstList instList;

	std::vector<ExceptionEntry> exceptions;

	Attrs attrs;

	virtual void write(BufferWriter& bw) {
		ASSERT(false, "");
		bw.writeu2(maxStack);
		bw.writeu2(maxLocals);

		u4 codeLen = -1;
		bw.writeu4(codeLen);

		u1* codeStart = bw.pos();
		bw.skip(codeLen);
		{
			//BufferWriter bw(codeStart, codeLen);
			//const std::basic_string<u1>& str = codeBuffer.str();
			//bw.writecount(str.c_str(), codeLen);
		}

		u2 esize = exceptions.size();
		bw.writeu2(esize);
		for (u4 i = 0; i < esize; i++) {
			ExceptionEntry& e = exceptions[i];
			bw.writeu2(e.startpc);
			bw.writeu2(e.endpc);
			bw.writeu2(e.handlerpc);
			bw.writeu2(e.catchtype);
		}

		attrs.writeAttrs(bw);
	}

	static void writeCode(BufferWriter&bw ) {

		void visitZero(int offset, u1 opcode) {
			line(offset, opcode);
		}

		inline void visitField(int offset, u1 opcode, u2 fieldRefIndex,
				const std::string& className, const std::string& name,
				const std::string& desc) {
			line(offset, opcode);
			writeu2(fieldRefIndex);
		}

		inline void visitBiPush(int offset, u1 opcode, u1 bytevalue) {
			line(offset, opcode);
			writeu1(bytevalue);
		}

		void visitSiPush(int offset, u1 opcode, u2 shortvalue) {
			line(offset, opcode);
			writeu2(shortvalue);
		}

		void visitNewArray(int offset, u1 opcode, u1 atype) {
			line(offset, opcode);
			writeu1(atype);
		}

		void visitType(int offset, u1 opcode, u2 classIndex,
				const std::string& className) {
			line(offset, opcode);
			writeu2(classIndex);
		}

		void visitJump(int offset, u1 opcode, u2 targetOffset) {
			line(offset, opcode);
			writeu2(targetOffset);
		}

		void visitMultiArray(int offset, u1 opcode, u2 classIndex,
				const std::string& className, u1 dims) {
			line(offset, opcode);
			writeu2(classIndex);
			writeu1(dims);
		}

		void visitIinc(int offset, u1 opcode, u1 index, u1 value) {
			line(offset, opcode);
			writeu1(index);
			writeu1(value);
		}

		void visitLdc(int offset, u1 opcode, u2 arg) {
			line(offset, opcode);

			if (opcode == OPCODE_ldc) {
				writeu1(arg);
			} else {
				writeu2(arg);
			}
		}

		void visitInvokeInterface(int offset, u1 opcode,
				u2 interMethodrefIndex, const std::string& className,
				const std::string& name, const std::string& desc,
				u1 count) {
			line(offset, opcode);
			writeu2(interMethodrefIndex);
			writeu1(count);
			writeu1(0);
		}

		void visitInvoke(int offset, u1 opcode, u2 methodrefIndex,
				const std::string& className, const std::string& name,
				const std::string& desc) {
			line(offset, opcode);
			writeu2(methodrefIndex);
		}

		void visitVar(int offset, u1 opcode, u2 lvindex) {
			line(offset, opcode);
			writeu1(lvindex);
		}

		void visitTableSwitch(int offset, u1 opcode, int def, int low,
				int high, const std::vector<u4>& targets) {
			line(offset, opcode);

			int pad = (4 - (pos() % 4)) % 4;
			for (int i = 0; i < pad; i++) {
				writeu1(0);
			}

			bool check = pos() % 4 == 0;
			ASSERT(check, "Padding offset must be mod 4: %d", pos());

			writeu4(def);
			writeu4(low);
			writeu4(high);

			for (int i = 0; i < high - low + 1; i++) {
				u4 t = targets[i];
				writeu4(t);
			}
		}

		void visitLookupSwitch(int offset, u1 opcode, u4 defbyte, u4 npairs,
				const std::vector<u4>& keys,
				const std::vector<u4>& targets) {
			line(offset, opcode);

			int pad = (4 - (pos() % 4)) % 4;
			for (int i = 0; i < pad; i++) {
				writeu1(0);
			}

			bool check = pos() % 4 == 0;
			ASSERT(check, "Padding offset must be mod 4: %d", pos());

			writeu4(defbyte);
			writeu4(npairs);

			for (u4 i = 0; i < npairs; i++) {
				u4 k = keys[i];
				writeu4(k);

				u4 t = targets[i];
				writeu4(t);
			}
		}
	}



};

}

#endif
