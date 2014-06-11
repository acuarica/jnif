/*
 * InstList.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_INSTLIST_HPP
#define JNIF_INSTLIST_HPP

#include "Inst.hpp"

namespace jnif {

/**
 * Represents the bytecode of a method.
 */
class InstList {
	friend class CodeAttr;

public:

	class Iterator {
		friend class InstList;
	public:

		Inst* operator*();

		Inst* operator->() const;

		bool friend operator==(const Iterator& lhs, const Iterator& rhs) {
			return lhs.position == rhs.position;
		}

		bool friend operator!=(const Iterator& lhs, const Iterator& rhs) {
			return lhs.position != rhs.position;
		}

		Iterator& operator++();

		Iterator& operator--();

	private:

		Iterator(Inst* position, Inst* last) :
				position(position), last(last) {
		}

		Inst* position;
		Inst* last;
	};

	LabelInst* createLabel();
	void addLabel(LabelInst* inst, Inst* pos = NULL);

	LabelInst* addLabel(Inst* pos = NULL);
	ZeroInst* addZero(Opcode opcode, Inst* pos = NULL);
	PushInst* addBiPush(u1 value, Inst* pos = NULL);
	PushInst* addSiPush(u2 value, Inst* pos = NULL);
	LdcInst* addLdc(Opcode opcode, ConstIndex valueIndex, Inst* pos = NULL);
	VarInst* addVar(Opcode opcode, u1 lvindex, Inst* pos = NULL);
	IincInst* addIinc(u1 index, u1 value, Inst* pos = NULL);
	WideInst* addWideVar(Opcode varOpcode, u2 lvindex, Inst* pos = NULL);
	WideInst* addWideIinc(u2 index, u2 value, Inst* pos = NULL);
	JumpInst* addJump(Opcode opcode, LabelInst* targetLabel, Inst* pos = NULL);
	FieldInst* addField(Opcode opcode, ConstIndex fieldRefIndex, Inst* pos =
	NULL);
	InvokeInst* addInvoke(Opcode opcode, ConstIndex methodRefIndex, Inst* pos =
	NULL);
	InvokeInterfaceInst* addInvokeInterface(ConstIndex interMethodRefIndex,
			u1 count, Inst* pos = NULL);
	InvokeDynamicInst* addInvokeDynamic(ConstIndex callSite, Inst* pos = NULL);
	TypeInst* addType(Opcode opcode, ConstIndex classIndex, Inst* pos = NULL);

	NewArrayInst* addNewArray(u1 atype, Inst* pos = NULL);
	MultiArrayInst* addMultiArray(ConstIndex classIndex, u1 dims, Inst* pos =
	NULL);

	TableSwitchInst* addTableSwitch(LabelInst* def, int low, int high,
			Inst* pos = NULL);

	LookupSwitchInst* addLookupSwitch(LabelInst* def, u4 npairs, Inst* pos =
	NULL);

	bool hasBranches() const {
		return branchesCount > 0;
	}

	bool hasJsrOrRet() const {
		return jsrOrRet;
	}

	int size() const {
		return _size;
	}

	Iterator begin() const {
		return Iterator(first, last);
	}

	Iterator end() const {
		return Iterator(NULL, last);
	}

	ClassFile* const constPool;

private:

	InstList(ClassFile* constPool) :
			constPool(constPool), first(NULL), last(NULL), _size(0), nextLabelId(
					1), branchesCount(0), jsrOrRet(false) {
	}

	~InstList();

	void addInst(Inst* inst, Inst* pos);

	Inst* first;
	Inst* last;

	int _size;

	int nextLabelId;

	int branchesCount;

	bool jsrOrRet;

	template<typename TInst, typename ... TArgs>
	TInst* _create(const TArgs& ... args);

//std::list<LabelInst*> _labelPool;

};

std::ostream& operator<<(std::ostream& os, const InstList& instList);

}

#endif
