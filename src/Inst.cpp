/*
 * Inst.cpp
 *
 *  Created on: May 16, 2014
 *      Author: luigi
 */
#include "jnif.hpp"
#include "jnifex.hpp"

namespace jnif {

void Inst::checkCast(bool cond, const char* kindName) const {
	//Error::assert(cond, "Inst is not a ", kindName, ": ", *this);
	Error::assert(cond, "Inst is not a ", kindName,
			": <missing instruction due to const>");
}

}
