/*
 * Frame.cpp
 *
 *  Created on: Jun 6, 2014
 *      Author: luigi
 */
#include "Frame.hpp"
#include "Error.hpp"
#include "jnif.hpp"
#include "analysis/SmtBuilder.hpp"
#include "analysis/ComputeFrames.hpp"
#include "analysis/ComputeFrames.hpp"

namespace jnif {

    static void setLink(std::set<Inst*>& is, Inst* j) {
        JnifError::assert(j != nullptr, "j cannot be null");
            for (Inst* i : is) {
                    JnifError::assert(i != nullptr, "i cannot be null");
                j->consumes.insert(i);
                i->produces.insert(j);
            }
    }

    Type Frame::getVar(u4 lvindex, Inst* inst) {
        JnifError::assert(inst != nullptr, "Inst cannot be null for getVar");

        T& t = lva.at(lvindex);
        setLink(t.second, inst);

        return t.first;
    }

Type Frame::pop(Inst* inst) {
	JnifError::check(stack.size() > 0, "Trying to pop in an empty stack.");
  JnifError::assert(inst != nullptr, "Inst cannot be null");

  Frame::T& t = stack.front();
  setLink(t.second, inst);
	stack.pop_front();

	return t.first;
}

Type Frame::popOneWord(Inst* inst) {
	Type t = pop(inst);
	JnifError::check(t.isOneWord() || t.isTop(), "Type is not one word type: ", t,
			", frame: ", *this);
	return t;
}

Type Frame::popTwoWord(Inst* inst) {
	Type t1 = pop(inst);
	Type t2 = pop(inst);

	JnifError::check(
			(t1.isOneWord() && t2.isOneWord()) || (t1.isTwoWord() && t2.isTop()),
			"Invalid types on top of the stack for pop2: ", t1, t2, *this);
	//Error::check(t2.isTop(), "Type is not Top type: ", t2, t1, *this);

	return t1;
}

Type Frame::popIntegral(Inst* inst) {
	Type t = popOneWord(inst);
	JnifError::assert(t.isIntegral(), "Invalid integral type on top of stack: ", t);
	return t;
}

Type Frame::popFloat(Inst* inst) {
	Type t = popOneWord(inst);
	JnifError::assert(t.isFloat(), "invalid float type on top of the stack");
	return t;
}

Type Frame::popLong(Inst* inst) {
	Type t = popTwoWord(inst);
	JnifError::check(t.isLong(), "invalid long type on top of the stack");
	return t;
}

Type Frame::popDouble(Inst* inst) {
	Type t = popTwoWord(inst);
	JnifError::check(t.isDouble(), "Invalid double type on top of the stack: ", t);

	return t;
}

    void Frame::popType(const Type& type, Inst* inst) {
	if (type.isIntegral()) {
		popIntegral(inst);
	} else if (type.isFloat()) {
		popFloat(inst);
	} else if (type.isLong()) {
		popLong(inst);
	} else if (type.isDouble()) {
		popDouble(inst);
	} else if (type.isObject()) {
		popRef(inst);
	} else {
		JnifError::raise("invalid pop type: ", type);
	}
}

    void Frame::push(const Type& t, Inst* inst) {
        std::set<Inst*> ls = {inst};
        stack.push_front(std::make_pair(t, ls));

  if (maxStack < stack.size()) {
    JnifError::assert(maxStack + 1 == stack.size(), "Invalid inc maxStack/size");
    maxStack++;
  }
}

    void Frame::pushType(const Type& type, Inst* inst) {
	if (type.isIntegral()) {
		pushInt(inst);
	} else if (type.isFloat()) {
		pushFloat(inst);
	} else if (type.isLong()) {
		pushLong(inst);
	} else if (type.isDouble()) {
		pushDouble(inst);
	} else if (type.isNull()) {
		pushNull(inst);
	} else if (type.isObject() || type.isUninitThis()) {
      push(type, inst);
	} else {
		JnifError::raise("invalid push type: ", type);
	}
}

    void Frame::setVar(u4* lvindex, const Type& t, Inst* inst) {
	JnifError::assert(t.isOneOrTwoWord(),
			"Setting var on non one-two word uninit this");

	if (t.isOneWord()) {
      _setVar(*lvindex, t, inst);
		(*lvindex)++;
	} else {
      _setVar(*lvindex, t, inst);
      _setVar(*lvindex + 1, _typeFactory->topType(), inst);
		(*lvindex) += 2;
	}
}

    void Frame::setVar2(u4 lvindex, const Type& t, Inst* inst) {
    setVar(&lvindex, t, inst);
}

    void Frame::setRefVar(u4 lvindex, const Type& type, Inst* inst) {
	JnifError::check(type.isObject() || type.isNull() || type.isUninitThis(),
			"Type must be object type: ", type);
	setVar(&lvindex, type, inst);
}

void Frame::cleanTops() {
	JnifError::assert(!topsErased, "tops already erased: ", topsErased);

	for (u4 i = 0; i < lva.size(); i++) {
		T t = lva[i];
		if (t.first.isTwoWord()) {
			T u = lva[i + 1];
      Type top = u.first;

			// workaround!!!
			if (top.isTop()) {
				JnifError::assert(top.isTop(), "Not top for two word: index: ", i,
						", top: ", top, " for ", t.first, " in ", *this);
				lva.erase(lva.begin() + i + 1);
			}
		}
	}

	for (int i = lva.size() - 1; i >= 0; i--) {
		T t = lva[i];
		if (t.first.isTop()) {

			lva.erase(lva.begin() + i);
		} else {
			return;
		}
	}
}

void Frame::join(Frame& how, IClassPath* classPath) {
  TypeFactory typeFactory;
  ComputeFrames comp;
  comp.join(typeFactory, *this, how, classPath);
}

    void Frame::_setVar(u4 lvindex, const Type& t, Inst* inst) {
	JnifError::check(lvindex < 256 * 256, "Index too large for LVA: ", lvindex);

	if (lvindex >= lva.size()) {
      lva.resize(lvindex + 1, std::make_pair(_typeFactory->topType(), std::set<Inst*>()));
	}

  std::set<Inst*> ls;
  if (inst != nullptr) {
      ls.insert(inst);
  }

	lva[lvindex] = std::make_pair(t, ls);
}

}
