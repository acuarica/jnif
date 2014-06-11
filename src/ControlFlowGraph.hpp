/*
 * ControlFlowGraph.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_CONTROLFLOWGRAPH_HPP
#define JNIF_CONTROLFLOWGRAPH_HPP

#include "BasicBlock.hpp"
namespace jnif {

/**
 * Represents a control flow graph of instructions.
 */
class ControlFlowGraph {
private:
	std::vector<BasicBlock*> basicBlocks;

public:
	BasicBlock* const entry;

	BasicBlock* const exit;

	const InstList& instList;

	ControlFlowGraph(InstList& instList, TypeFactory& typeFactory);

	~ControlFlowGraph();

	/**
	 * Adds a basic block to this control flow graph.
	 *
	 * @param start the start of the basic block.
	 * @param end the end of the basic block.
	 * @param name the name of the basic block to add.
	 * @returns the newly created basic block added to this control flow graph.
	 */
	BasicBlock* addBasicBlock(InstList::Iterator start, InstList::Iterator end,
			const String& name);

	/**
	 * Finds the basic block associated with the given labelId.
	 *
	 * @param labelId the label id to search in the basic blocks.
	 * @returns the basic block that starts at label with labelId. If the label
	 * id is not found throws an exception.
	 */
	BasicBlock* findBasicBlockOfLabel(int labelId) const;

	std::vector<BasicBlock*>::iterator begin() {
		return basicBlocks.begin();
	}

	std::vector<BasicBlock*>::iterator end() {
		return basicBlocks.end();
	}

	std::vector<BasicBlock*>::const_iterator begin() const {
		return basicBlocks.begin();
	}

	std::vector<BasicBlock*>::const_iterator end() const {
		return basicBlocks.end();
	}

private:
	BasicBlock* addConstBb(InstList& instList, const char* name) {
		return addBasicBlock(instList.end(), instList.end(), name);
	}

	TypeFactory& _typeFactory;

};

std::ostream& operator<<(std::ostream& os, BasicBlock& bb);
std::ostream& operator<<(std::ostream& os, const ControlFlowGraph& cfg);

}

#endif
