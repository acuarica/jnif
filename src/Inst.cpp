/*
 * Inst.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: luigi
 */
#include "Inst.hpp"
#include "Error.hpp"

namespace jnif {

Inst::~Inst() {
  Error::trace("~Inst");
}

LabelInst::~LabelInst() {
  Error::trace("~LabelInst");
}

ZeroInst::~ZeroInst() {
  Error::trace("~ZeroInst");
}

SwitchInst::~SwitchInst() {
  Error::trace("~SwitchInst");
}

TableSwitchInst::~TableSwitchInst() {
  Error::trace("~TableSwitchInst");
}

LookupSwitchInst::~LookupSwitchInst() {
  Error::trace("~LookupSwitchInst");
}

void Inst::checkCast(bool cond, const char* kindName) const {
	Error::assert(cond, "Inst is not a ", kindName, ": ", *this);
}

}
