/*
 * Frame.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_FRAME_HPP
#define JNIF_FRAME_HPP

#include "Type.hpp"
#include "TypeFactory.hpp"

#include <list>
#include <set>
#include <utility>

namespace jnif {

/**
 *
 */
class Frame {

	//Frame(const Frame&) = delete;

public:

	Frame(TypeFactory* typeFactory) :
    valid(false), topsErased(false), maxStack(0), _typeFactory(typeFactory) {
		lva.reserve(256);
	}

	Type pop(Inst* inst);

	Type popOneWord(Inst* inst);

	Type popTwoWord(Inst* inst);

	Type popIntegral(Inst* inst);

	Type popFloat(Inst* inst);

	Type popLong(Inst* inst);

	Type popDouble(Inst* inst);

	Type popRef(Inst* inst) {
		Type t = popOneWord(inst);
		//assert(t.is(), "invalid ref type on top of the stack");
		return t;
	}

	Type popArray(Inst* inst) {
		return popOneWord(inst);
	}

    void popType(const Type& type, Inst* inst);

    void pushType(const Type& type, Inst* inst);

    void push(const Type& t, Inst* inst);

	void pushInt(Inst* inst) {
      push(_typeFactory->intType(), inst);
	}
	void pushLong(Inst* inst) {
      push(_typeFactory->topType(), inst);
      push(_typeFactory->longType(), inst);
	}
	void pushFloat(Inst* inst) {
      push(_typeFactory->floatType(), inst);
	}
	void pushDouble(Inst* inst) {
      push(_typeFactory->topType(), inst);
      push(_typeFactory->doubleType(), inst);
	}

    void pushRef(const String& className, Inst* inst) {
        push(_typeFactory->objectType(className), inst);
	}

    void pushArray(const Type& type, u4 dims, Inst* inst) {
      push(_typeFactory->arrayType(type, dims), inst);
	}

	void pushNull(Inst* inst) {
      push(_typeFactory->nullType(), inst);
	}

    Type getVar(u4 lvindex, Inst* inst);

    void setVar(u4* lvindex, const Type& t, Inst* inst);

    void setVar2(u4 lvindex, const Type& t, Inst* inst);

    void setIntVar(u4 lvindex, Inst* inst) {
        setVar(&lvindex, _typeFactory->intType(), inst);
	}

    void setLongVar(u4 lvindex, Inst* inst) {
        setVar(&lvindex, _typeFactory->longType(), inst);
	}

    void setFloatVar(u4 lvindex, Inst* inst) {
        setVar(&lvindex, _typeFactory->floatType(), inst);
	}

    void setDoubleVar(u4 lvindex, Inst* inst) {
        setVar(&lvindex, _typeFactory->doubleType(), inst);
	}

    void setRefVar(u4 lvindex, const String& className, Inst* inst) {
        setVar(&lvindex, _typeFactory->objectType(className), inst);
	}

    void setRefVar(u4 lvindex, const Type& type, Inst* inst);

	void clearStack() {
		stack.clear();
	}

	void cleanTops();

	void join(Frame& how, class IClassPath* classPath);

    typedef std::pair<Type, std::set<Inst*> > T;

    std::vector<T> lva;
    std::list<T> stack;
	bool valid;
	bool topsErased;

  unsigned long maxStack;

	friend bool operator ==(const Frame& lhs, const Frame& rhs) {
		return lhs.lva == rhs.lva && lhs.stack == rhs.stack
				&& lhs.valid == rhs.valid;
	}

private:

    void _setVar(u4 lvindex, const Type& t, Inst* inst);

	TypeFactory* _typeFactory;

};

std::ostream& operator<<(std::ostream& os, const Frame& frame);

}

#endif
