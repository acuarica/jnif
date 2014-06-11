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

namespace jnif {

/**
 *
 */
class Frame {

	//Frame(const Frame&) = delete;

public:

	Frame(TypeFactory* typeFactory) :
			valid(false), topsErased(false), _typeFactory(typeFactory) {
		lva.reserve(256);
	}

	Type pop();

	Type popOneWord();

	Type popTwoWord();

	Type popIntegral();

	Type popFloat();

	Type popLong();

	Type popDouble();

	Type popRef() {
		Type t = popOneWord();
		//assert(t.is(), "invalid ref type on top of the stack");
		return t;
	}

	Type popArray() {
		return popOneWord();
	}

	void popType(const Type& type);

	void pushType(const Type& type);

	void push(const Type& t) {
		stack.push_front(t);
	}

	void pushInt() {
		push(_typeFactory->intType());
	}
	void pushLong() {
		push(_typeFactory->topType());
		push(_typeFactory->longType());
	}
	void pushFloat() {
		push(_typeFactory->floatType());
	}
	void pushDouble() {
		push(_typeFactory->topType());
		push(_typeFactory->doubleType());
	}

	void pushRef(const String& className) {
		push(_typeFactory->objectType(className));
	}

	void pushArray(const Type& type, u4 dims) {
		push(_typeFactory->arrayType(type, dims));
	}

	void pushNull() {
		push(_typeFactory->nullType());
	}

	const Type& getVar(u4 lvindex) {
		return lva.at(lvindex);
	}

	void setVar(u4* lvindex, const Type& t);

	void setVar2(u4 lvindex, const Type& t);

	void setIntVar(u4 lvindex) {
		setVar(&lvindex, _typeFactory->intType());
	}

	void setLongVar(u4 lvindex) {
		setVar(&lvindex, _typeFactory->longType());
	}

	void setFloatVar(u4 lvindex) {
		setVar(&lvindex, _typeFactory->floatType());
	}

	void setDoubleVar(u4 lvindex) {
		setVar(&lvindex, _typeFactory->doubleType());
	}

	void setRefVar(u4 lvindex, const String& className) {
		setVar(&lvindex, _typeFactory->objectType(className));
	}

	void setRefVar(u4 lvindex, const Type& type);

	void clearStack() {
		stack.clear();
	}

	void cleanTops();

	void join(Frame& how, class IClassPath* classPath);

	std::vector<Type> lva;
	std::list<Type> stack;
	bool valid;
	bool topsErased;

	friend bool operator ==(const Frame& lhs, const Frame& rhs) {
		return lhs.lva == rhs.lva && lhs.stack == rhs.stack
				&& lhs.valid == rhs.valid;
	}

private:

	void _setVar(u4 lvindex, const Type& t);

	TypeFactory* _typeFactory;

};

std::ostream& operator<<(std::ostream& os, const Frame& frame);

}

#endif
