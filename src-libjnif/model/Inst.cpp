/*
 * Inst.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: luigi
 */
#include "Inst.hpp"
#include "../Error.hpp"

namespace jnif::model {

void Inst::checkCast(bool cond, const char* kindName) const {
	JnifError::assert(cond, "Inst is not a ", kindName, ": ", *this);
}

}
