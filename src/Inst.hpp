/*
 * Inst.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_INST_HPP
#define JNIF_INST_HPP

#include "base.hpp"
#include "Opcode.hpp"
#include "ConstPool.hpp"

namespace jnif {

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
	KIND_RESERVED,
	KIND_LABEL,
	KIND_FRAME
};

class ClassFile;

/**
 * Represent a bytecode instruction.
 */
class Inst {
	friend class LabelInst;
	friend class ZeroInst;
	friend class PushInst;
	friend class LdcInst;
	friend class VarInst;
	friend class IincInst;
	friend class WideInst;
	friend class JumpInst;
	friend class FieldInst;
	friend class InvokeInst;
	friend class InvokeInterfaceInst;
	friend class InvokeDynamicInst;
	friend class TypeInst;
	friend class NewArrayInst;
	friend class MultiArrayInst;
	friend class SwitchInst;
	friend class InstList;

public:

	bool isJump() const {
		return kind == KIND_JUMP;
	}

	bool isTableSwitch() const {
		return kind == KIND_TABLESWITCH;
	}

	bool isLookupSwitch() const {
		return kind == KIND_LOOKUPSWITCH;
	}

	bool isBranch() const {
		return isJump() || isTableSwitch() || isLookupSwitch();
	}

	bool isExit() const {
		return (opcode >= OPCODE_ireturn && opcode <= OPCODE_return)
				|| opcode == OPCODE_athrow;
	}

	bool isLabel() const {
		return kind == KIND_LABEL;
	}

	bool isPush() const {
		return kind == KIND_BIPUSH || kind == KIND_SIPUSH;
	}

	bool isLdc() const {
		return kind == KIND_LDC;
	}

	bool isVar() const {
		return kind == KIND_VAR;
	}

	bool isIinc() const {
		return kind == KIND_IINC;
	}

	bool isInvoke() const {
		return kind == KIND_INVOKE;
	}

	/**
	 * Returns true is this instruction is an invokeinterface instruction.
	 * False otherwise.
	 */
	bool isInvokeInterface() const {
		return kind == KIND_INVOKEINTERFACE;
	}

	/**
	 * Returns true is this instruction is an invokedynamic instruction.
	 * False otherwise.
	 */
	bool isInvokeDynamic() const {
		return kind == KIND_INVOKEDYNAMIC;
	}

	bool isType() const {
		return kind == KIND_TYPE;
	}
	bool isNewArray() const {
		return kind == KIND_NEWARRAY;
	}
	bool isWide() const {
		return opcode == OPCODE_wide && kind == KIND_ZERO;
	}
	bool isField() const {
		return kind == KIND_FIELD;
	}
	bool isMultiArray() const {
		return kind == KIND_MULTIARRAY;
	}

	bool isJsrOrRet() const {
		return opcode == OPCODE_jsr || opcode == OPCODE_jsr_w
				|| opcode == OPCODE_ret;
	}

	/**
	 * The opcode of this instruction.
	 */
	const Opcode opcode;

	/**
	 * The kind of this instruction.
	 */
	const OpKind kind;

	int _offset;

	const ClassFile* const constPool;
	Inst* prev;
	Inst* next;

	class LabelInst* label() {
		return cast<LabelInst>(isLabel(), "label");
	}

	class PushInst* push() {
		return cast<PushInst>(isPush(), "push");
	}

	class LdcInst* ldc() {
		return cast<LdcInst>(isLdc(), "ldc");
	}

	class VarInst* var() {
		return cast<VarInst>(isVar(), "var");
	}

	class IincInst* iinc() {
		return cast<IincInst>(isIinc(), "iinc");
	}

	class InvokeInst* invoke() {
		return cast<InvokeInst>(isInvoke(), "invoke");
	}

	class JumpInst* jump() {
		return cast<JumpInst>(isJump(), "jump");
	}

	class TableSwitchInst* ts() {
		return cast<TableSwitchInst>(isTableSwitch(), "ts");
	}

	class LookupSwitchInst* ls() {
		return cast<LookupSwitchInst>(isLookupSwitch(), "ls");
	}

	class InvokeInterfaceInst* invokeinterface() {
		return cast<InvokeInterfaceInst>(isInvokeInterface(), "invinter");
	}

	class TypeInst* type() {
		return cast<TypeInst>(isType(), "type");
	}

	class NewArrayInst* newarray() {
		return cast<NewArrayInst>(isNewArray(), "newarray");
	}

	class WideInst* wide() {
		return cast<WideInst>(isWide(), "wide");
	}

	class FieldInst* field() {
		return cast<FieldInst>(isField(), "field");
	}

	class MultiArrayInst* multiarray() {
		return cast<MultiArrayInst>(isMultiArray(), "multiarray");
	}

	class LabelInst* label() const {
		return cast<LabelInst>(isLabel(), "label");
	}

	class PushInst* push() const {
		return cast<PushInst>(isPush(), "push");
	}

	class LdcInst* ldc() const {
		return cast<LdcInst>(isLdc(), "ldc");
	}

	class VarInst* var() const {
		return cast<VarInst>(isVar(), "var");
	}

	class IincInst* iinc() const {
		return cast<IincInst>(isIinc(), "iinc");
	}

	class InvokeInst* invoke() const {
		return cast<InvokeInst>(isInvoke(), "invoke");
	}

	class JumpInst* jump() const {
		return cast<JumpInst>(isJump(), "jump");
	}

	class TableSwitchInst* ts() const {
		return cast<TableSwitchInst>(isTableSwitch(), "ts");
	}

	class LookupSwitchInst* ls() const {
		return cast<LookupSwitchInst>(isLookupSwitch(), "ls");
	}

	class InvokeInterfaceInst* invokeinterface() const {
		return cast<InvokeInterfaceInst>(isInvokeInterface(), "invinter");
	}

	class InvokeDynamicInst* indy() const {
		return cast<InvokeDynamicInst>(isInvokeDynamic(), "indy");
	}

	class TypeInst* type() const {
		return cast<TypeInst>(isType(), "type");
	}

	class NewArrayInst* newarray() const {
		return cast<NewArrayInst>(isNewArray(), "newarray");
	}

	class WideInst* wide() const {
		return cast<WideInst>(isWide(), "wide");
	}

	class FieldInst* field() const {
		return cast<FieldInst>(isField(), "field");
	}

	class MultiArrayInst* multiarray() const {
		return cast<MultiArrayInst>(isMultiArray(), "multiarray");
	}

private:

	Inst() :
			opcode(OPCODE_nop), kind(KIND_ZERO), _offset(0), constPool(NULL), prev(
			NULL), next(NULL) {
	}

	Inst(Opcode opcode, OpKind kind, ClassFile* constPool, Inst* prev = NULL,
			Inst* next = NULL) :
			opcode(opcode), kind(kind), _offset(0), constPool(constPool), prev(
					prev), next(next) {
	}

	virtual ~Inst();

	template<typename TKind>
	TKind* cast(bool cond, const char* kindName) {
		checkCast(cond, kindName);
		return (TKind*) this;
	}

	template<typename TKind>
	TKind* cast(bool cond, const char* kindName) const {
		checkCast(cond, kindName);
		return (TKind*) this;
	}

	void checkCast(bool cond, const char* kindName) const;
};

/**
 *
 */
class LabelInst: public Inst {
	friend class InstList;
public:

	u2 offset;
	u2 deltaOffset;
	int id;
	bool isBranchTarget;
	bool isTryStart;
	bool isTryEnd;
	bool isCatchHandler;

private:

	virtual ~LabelInst();

	LabelInst(ClassFile* constPool, int id) :
			Inst(OPCODE_nop, KIND_LABEL, constPool), offset(0), deltaOffset(0), id(
					id), isBranchTarget(false), isTryStart(false), isTryEnd(
					false), isCatchHandler(false) {
	}

};

/**
 *
 */
class ZeroInst: public Inst {
	friend class InstList;

private:

	ZeroInst(Opcode opcode, ClassFile* constPool) :
			Inst(opcode, KIND_ZERO, constPool) {
	}

	virtual ~ZeroInst();
};

/**
 *
 */
class PushInst: public Inst {
	friend class InstList;

public:
	int value;

private:

	PushInst(Opcode opcode, OpKind kind, int value, ClassFile* constPool) :
			Inst(opcode, kind, constPool), value(value) {
	}
};

/**
 *
 */
class LdcInst: public Inst {
	friend class InstList;

public:
	ConstIndex valueIndex;

private:

	LdcInst(Opcode opcode, ConstIndex valueIndex, ClassFile* constPool) :
			Inst(opcode, KIND_LDC, constPool), valueIndex(valueIndex) {
	}
};

/**
 *
 */
class VarInst: public Inst {
	friend class InstList;

public:
	u1 lvindex;

private:

	VarInst(Opcode opcode, u1 lvindex, ClassFile* constPool) :
			Inst(opcode, KIND_VAR, constPool), lvindex(lvindex) {
	}
};

/**
 *
 */
class IincInst: public Inst {
	friend class InstList;

public:
	u1 index;
	u1 value;

private:

	IincInst(u1 index, u1 value, ClassFile* constPool) :
			Inst(OPCODE_iinc, KIND_IINC, constPool), index(index), value(value) {
	}

};

/**
 *
 */
class WideInst: public Inst {
	friend class InstList;

public:
	Opcode subOpcode;

	union {
		struct {
			u2 lvindex;
		} var;
		struct {
			u2 index;
			u2 value;
		} iinc;
	};

private:

	WideInst(Opcode subOpcode, u2 lvindex, ClassFile* constPool) :
			Inst(OPCODE_wide, KIND_ZERO, constPool), subOpcode(subOpcode) {
		if (subOpcode == OPCODE_ret) {
			throw JnifException("Ret found in wide instruction!!!", "no bt");
		}

		var.lvindex = lvindex;
	}

	WideInst(u2 index, u2 value, ClassFile* constPool) :
			Inst(OPCODE_wide, KIND_ZERO, constPool), subOpcode(OPCODE_iinc) {
		iinc.index = index;
		iinc.value = value;
	}

};

/**
 *
 */
class JumpInst: public Inst {
	friend class InstList;

public:

	Inst* label2;

private:

	JumpInst(Opcode opcode, LabelInst* targetLabel, ClassFile* constPool) :
			Inst(opcode, KIND_JUMP, constPool), label2(targetLabel) {
	}
};

/**
 *
 */
class FieldInst: public Inst {
	friend class InstList;

public:
	ConstIndex fieldRefIndex;

private:

	FieldInst(Opcode opcode, ConstIndex fieldRefIndex, ClassFile* constPool) :
			Inst(opcode, KIND_FIELD, constPool), fieldRefIndex(fieldRefIndex) {
	}
};

/**
 *
 */
class InvokeInst: public Inst {
	friend class InstList;

public:
	ConstIndex methodRefIndex;

private:

	InvokeInst(Opcode opcode, ConstIndex methodRefIndex, ClassFile* constPool) :
			Inst(opcode, KIND_INVOKE, constPool), methodRefIndex(methodRefIndex) {
	}
};

/**
 *
 */
class InvokeInterfaceInst: public Inst {
	friend class InstList;

public:
	u2 interMethodRefIndex;
	u1 count;

private:

	InvokeInterfaceInst(ConstIndex interMethodRefIndex, u1 count,
			ClassFile* constPool) :
			Inst(OPCODE_invokeinterface, KIND_INVOKEINTERFACE, constPool), interMethodRefIndex(
					interMethodRefIndex), count(count) {
	}
};

/**
 * Represents an invokedynamic bytecode.
 */
class InvokeDynamicInst: public Inst {
	friend class InstList;

public:

	/**
	 * Returns the call site for this invokedynamic instruction.
	 */
	ConstIndex callSite() const {
		return _callSite;
	}

private:

	InvokeDynamicInst(ConstIndex callSite, ClassFile* constPool) :
			Inst(OPCODE_invokedynamic, KIND_INVOKEDYNAMIC, constPool), _callSite(
					callSite) {
	}

	ConstIndex _callSite;
};

/**
 *
 */
class TypeInst: public Inst {
	friend class InstList;

public:

	/**
	 * Index in the constant pool of a class entry.
	 */
	ConstIndex classIndex;

private:

	TypeInst(Opcode opcode, ConstIndex classIndex, ClassFile* constPool) :
			Inst(opcode, KIND_TYPE, constPool), classIndex(classIndex) {
	}

};

/**
 *
 */
class NewArrayInst: public Inst {
	friend class InstList;

public:
	u1 atype;

private:

	NewArrayInst(Opcode opcode, u1 atype, ClassFile* constPool) :
			Inst(opcode, KIND_NEWARRAY, constPool), atype(atype) {
	}

};

/**
 *
 */
class MultiArrayInst: public Inst {
	friend class InstList;

public:
	ConstIndex classIndex;
	u1 dims;

private:

	MultiArrayInst(Opcode opcode, ConstIndex classIndex, u1 dims,
			ClassFile* constPool) :
			Inst(opcode, KIND_MULTIARRAY, constPool), classIndex(classIndex), dims(
					dims) {
	}

};

/**
 * Base class for TableSwitchInst and LookupSwitchInst.
 */
class SwitchInst: public Inst {
	friend class TableSwitchInst;
	friend class LookupSwitchInst;

public:

	std::vector<Inst*> targets;

	void addTarget(LabelInst* label) {
		targets.push_back(label);
		label->isBranchTarget = true;
	}

private:

	SwitchInst(Opcode opcode, OpKind kind, ClassFile* constPool) :
			Inst(opcode, kind, constPool) {
	}

};

/**
 *
 */
class TableSwitchInst: public SwitchInst {
	friend class InstList;

public:

	Inst* def;
	int low;
	int high;

private:

	TableSwitchInst(LabelInst* def, int low, int high, ClassFile* constPool) :
			SwitchInst(OPCODE_tableswitch, KIND_TABLESWITCH, constPool), def(
					def), low(low), high(high) {
	}

};

/**
 *
 */
class LookupSwitchInst: public SwitchInst {
	friend class InstList;

public:
	Inst* defbyte;
	u4 npairs;
	std::vector<u4> keys;
	//std::vector<Inst*> targets;

private:

	LookupSwitchInst(LabelInst* def, u4 npairs, ClassFile* constPool) :
			SwitchInst(OPCODE_lookupswitch, KIND_LOOKUPSWITCH, constPool), defbyte(
					def), npairs(npairs) {
	}
};

std::ostream& operator<<(std::ostream& os, const Inst& inst);

}

#endif
