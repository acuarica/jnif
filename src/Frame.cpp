/*
 * Frame.cpp
 *
 *  Created on: Jun 6, 2014
 *      Author: luigi
 */
#include "Frame.hpp"
#include "Error.hpp"

namespace jnif {

Type Frame::pop() {
	JnifError::check(stack.size() > 0, "Trying to pop in an empty stack.");

	Type t = stack.front();
	stack.pop_front();
	return t;
}

Type Frame::popOneWord() {
	Type t = pop();
	JnifError::check(t.isOneWord() || t.isTop(), "Type is not one word type: ", t,
			", frame: ", *this);
	return t;
}

Type Frame::popTwoWord() {
	Type t1 = pop();
	Type t2 = pop();

	JnifError::check(
			(t1.isOneWord() && t2.isOneWord()) || (t1.isTwoWord() && t2.isTop()),
			"Invalid types on top of the stack for pop2: ", t1, t2, *this);
	//Error::check(t2.isTop(), "Type is not Top type: ", t2, t1, *this);

	return t1;
}

Type Frame::popIntegral() {
	Type t = popOneWord();
	JnifError::assert(t.isIntegral(), "Invalid integral type on top of stack: ", t);
	return t;
}

Type Frame::popFloat() {
	Type t = popOneWord();
	JnifError::assert(t.isFloat(), "invalid float type on top of the stack");
	return t;
}

Type Frame::popLong() {
	Type t = popTwoWord();
	JnifError::check(t.isLong(), "invalid long type on top of the stack");
	return t;
}

Type Frame::popDouble() {
	Type t = popTwoWord();
	JnifError::check(t.isDouble(), "Invalid double type on top of the stack: ", t);

	return t;
}

void Frame::popType(const Type& type) {
	if (type.isIntegral()) {
		popIntegral();
	} else if (type.isFloat()) {
		popFloat();
	} else if (type.isLong()) {
		popLong();
	} else if (type.isDouble()) {
		popDouble();
	} else if (type.isObject()) {
		popRef();
	} else {
		JnifError::raise("invalid pop type: ", type);
	}
}

void Frame::push(const Type& t) {
  stack.push_front(t);

  if (maxStack < stack.size()) {
    JnifError::assert(maxStack + 1 == stack.size(), "Invalid inc maxStack/size");
    maxStack++;
  }
}

void Frame::pushType(const Type& type) {
	if (type.isIntegral()) {
		pushInt();
	} else if (type.isFloat()) {
		pushFloat();
	} else if (type.isLong()) {
		pushLong();
	} else if (type.isDouble()) {
		pushDouble();
	} else if (type.isNull()) {
		pushNull();
	} else if (type.isObject() || type.isUninitThis()) {
		push(type);
	} else {
		JnifError::raise("invalid push type: ", type);
	}
}

void Frame::setVar(u4* lvindex, const Type& t) {
	JnifError::assert(t.isOneOrTwoWord(),
			"Setting var on non one-two word uninit this");

	if (t.isOneWord()) {
		_setVar(*lvindex, t);
		(*lvindex)++;
	} else {
		_setVar(*lvindex, t);
		_setVar(*lvindex + 1, _typeFactory->topType());
		(*lvindex) += 2;
	}
}

void Frame::setVar2(u4 lvindex, const Type& t) {
	setVar(&lvindex, t);
}

void Frame::setRefVar(u4 lvindex, const Type& type) {
	JnifError::check(type.isObject() || type.isNull() || type.isUninitThis(),
			"Type must be object type: ", type);
	setVar(&lvindex, type);
}

void Frame::cleanTops() {
	JnifError::assert(!topsErased, "tops already erased: ", topsErased);

	for (u4 i = 0; i < lva.size(); i++) {
		Type t = lva[i];
		if (t.isTwoWord()) {
			Type top = lva[i + 1];

			// workaround!!!
			if (top.isTop()) {
				JnifError::assert(top.isTop(), "Not top for two word: index: ", i,
						", top: ", top, " for ", t, " in ", *this);
				lva.erase(lva.begin() + i + 1);
			}
		}
	}

	for (int i = lva.size() - 1; i >= 0; i--) {
		Type t = lva[i];
		if (t.isTop()) {

			lva.erase(lva.begin() + i);
		} else {
			return;
		}
	}
}

void Frame::_setVar(u4 lvindex, const Type& t) {
	JnifError::check(lvindex < 256 * 256, "Index too large for LVA: ", lvindex);

	if (lvindex >= lva.size()) {
		lva.resize(lvindex + 1, _typeFactory->topType());
	}

	lva[lvindex] = t;
}

}
