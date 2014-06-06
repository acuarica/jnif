/*
 * BasicBlock.cpp
 *
 *  Created on: Jun 6, 2014
 *      Author: luigi
 */
#include "jnif.hpp"

namespace jnif {

void BasicBlock::addTarget(BasicBlock* target) {
	Error::check(cfg == target->cfg, "invalid owner for basic block");

	targets.push_back(target);
}

}
