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
}

void Inst::checkCast(bool cond, const char* kindName) const {
	Error::assert(cond, "Inst is not a ", kindName, ": ", *this);
	//Error::assert(cond, "Inst is not a ", kindName,
	//	": <missing instruction due to const>");
}

}
