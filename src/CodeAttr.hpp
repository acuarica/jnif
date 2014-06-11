/*
 * CodeAttr.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_CODEATTR_HPP
#define JNIF_CODEATTR_HPP

#include "Inst.hpp"
#include "InstList.hpp"
#include "Type.hpp"
#include "ControlFlowGraph.hpp"

namespace jnif {

class ClassFile;

/**
 * Defines the base class for all attributes in the class file.
 */
class Attr {

	Attr(const Attr&) = delete;
public:

	AttrKind kind;

	u2 nameIndex;
	u4 len;
	ClassFile* const constPool;

//	virtual void accept(Visitor* v) = 0;

	virtual ~Attr() {
	}

protected:

	Attr(AttrKind kind, u2 nameIndex, u4 len, ClassFile* constPool) :
			kind(kind), nameIndex(nameIndex), len(len), constPool(constPool) {
	}

};

/**
 * Represents a collection of attributes within a class, method or field
 * or even with another attributes, e.g., CodeAttr.
 */
class Attrs {
public:

	Attrs(const Attrs&) = delete;
	Attrs(Attrs&&) = default;

	Attrs() {
	}

	~Attrs();

	Attr* add(Attr* attr) {
		attrs.push_back(attr);

		return attr;
	}

	u2 size() const {
		return attrs.size();
	}

	const Attr& operator[](u2 index) const {
		return *attrs[index];
	}

	std::vector<Attr*>::iterator begin() {
		return attrs.begin();
	}

	std::vector<Attr*>::iterator end() {
		return attrs.end();
	}

	std::vector<Attr*>::const_iterator begin() const {
		return attrs.begin();
	}

	std::vector<Attr*>::const_iterator end() const {
		return attrs.end();
	}

	std::vector<Attr*> attrs;
};

/**
 * Represents an unknown opaque attribute to jnif.
 */
class UnknownAttr: public Attr {
public:

	const u1 * const data;

	UnknownAttr(u2 nameIndex, u4 len, const u1* data, ClassFile* constPool) :
			Attr(ATTR_UNKNOWN, nameIndex, len, constPool), data(data) {
	}

//	void accept(Visitor* v) {
//		v->visit(*this);
//	}
};

/**
 * Represents the LineNumberTable attribute within the Code attribute.
 */
class LvtAttr: public Attr {
public:

	struct LvEntry {
		u2 startPc;
		Inst* startPcLabel;

		Inst* endPcLabel;

		u2 len;
		u2 varNameIndex;
		u2 varDescIndex;
		u2 index;
	};

	std::vector<LvEntry> lvt;

	LvtAttr(AttrKind kind, u2 nameIndex, ClassFile* constPool) :
			Attr(kind, nameIndex, 0, constPool) {
	}
};

/**
 * Represents the LineNumberTable attribute within the Code attribute.
 */
class LntAttr: public Attr {
public:

	LntAttr(u2 nameIndex, ClassFile* constPool) :
			Attr(ATTR_LNT, nameIndex, 0, constPool) {
	}

	struct LnEntry {
		u2 startpc;
		Inst* startPcLabel;

		u2 lineno;
	};

	std::vector<LnEntry> lnt;

};

/**
 *
 */
class SmtAttr: public Attr {
public:

	SmtAttr(u2 nameIndex, ClassFile* constPool) :
			Attr(ATTR_SMT, nameIndex, 0, constPool) {
	}

	class Entry {
	public:

		int frameType;
		Inst* label;

		struct {
		} sameFrame;
		struct {
			std::vector<Type> stack; // [1]
		} sameLocals_1_stack_item_frame;
		struct {
			short offset_delta;
			std::vector<Type> stack; // [1]
		} same_locals_1_stack_item_frame_extended;
		struct {
			short offset_delta;
		} chop_frame;
		struct {
			short offset_delta;
		} same_frame_extended;
		struct {
			short offset_delta;
			std::vector<Type> locals; // frameType - 251
		} append_frame;
		struct {
			short offset_delta;
			std::vector<Type> locals;
			std::vector<Type> stack;
		} full_frame;
	};

	std::vector<Entry> entries;
};

/**
 * Represents the Exceptions attribute.
 */
class ExceptionsAttr: public Attr {
public:

	ExceptionsAttr(u2 nameIndex, ClassFile* constPool,
			const std::vector<u2>& es) :
			Attr(ATTR_EXCEPTIONS, nameIndex, es.size() * 2 + 2, constPool), es(
					es) {
	}

	std::vector<ConstIndex> es;
};

/**
 *
 */
struct CodeExceptionEntry {
	Inst* startpc;
	Inst* endpc;
	Inst* handlerpc;
	ConstIndex catchtype;
};

/**
 * Represent the Code attribute of a method.
 */
class CodeAttr: public Attr {
public:

	CodeAttr(u2 nameIndex, ClassFile* constPool) :
			Attr(ATTR_CODE, nameIndex, 0, constPool), maxStack(0), maxLocals(0), codeLen(
					-1), instList(constPool), cfg(NULL) {
	}

	~CodeAttr();

	u2 maxStack;
	u2 maxLocals;
	u4 codeLen;

	InstList instList;

	bool hasTryCatch() const {
		return exceptions.size() > 0;
	}

	std::vector<CodeExceptionEntry> exceptions;

	ControlFlowGraph* cfg;

	Attrs attrs;
};

/**
 *
 */
class SourceFileAttr: public Attr {
public:

	const ConstIndex sourceFileIndex;

	SourceFileAttr(ConstIndex nameIndex, ConstIndex sourceFileIndex,
			ClassFile* constPool) :
			Attr(ATTR_SOURCEFILE, nameIndex, 2, constPool), sourceFileIndex(
					sourceFileIndex) {
	}

};

}

#endif
