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
  JnifError::trace("~Inst");
}

LabelInst::~LabelInst() {
  JnifError::trace("~LabelInst");
}

ZeroInst::~ZeroInst() {
  JnifError::trace("~ZeroInst");
}

SwitchInst::~SwitchInst() {
  JnifError::trace("~SwitchInst");
}

TableSwitchInst::~TableSwitchInst() {
  JnifError::trace("~TableSwitchInst");
}

LookupSwitchInst::~LookupSwitchInst() {
  JnifError::trace("~LookupSwitchInst");
}

void Inst::checkCast(bool cond, const char* kindName) const {
	JnifError::assert(cond, "Inst is not a ", kindName, ": ", *this);
}

}
