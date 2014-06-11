/*
 * BasicBlock.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_BASICBLOCK_HPP
#define JNIF_BASICBLOCK_HPP
#include "InstList.hpp"
#include "Frame.hpp"

namespace jnif {

/**
 * Represents a basic block of instructions.
 *
 * @see Inst
 */
class BasicBlock {
public:
	BasicBlock(const BasicBlock&) = delete;
	BasicBlock(BasicBlock&&) = default;

	friend class ControlFlowGraph;

	void addTarget(BasicBlock* target);

	InstList::Iterator start;
	InstList::Iterator exit;
	String name;
	Frame in;
	Frame out;

	std::vector<BasicBlock*>::iterator begin() {
		return targets.begin();
	}

	std::vector<BasicBlock*>::iterator end() {
		return targets.end();
	}

	BasicBlock* next;
	class ControlFlowGraph* cfg;

private:

	BasicBlock(InstList::Iterator& start, InstList::Iterator& exit,
			const String& name, class ControlFlowGraph* cfg,
			TypeFactory& typeFactory) :
			start(start), exit(exit), name(name), in(&typeFactory), out(
					&typeFactory), next(NULL), cfg(cfg) {
	}

	std::vector<BasicBlock*> targets;
};

}

#endif /* BASICBLOCK_HPP_ */
