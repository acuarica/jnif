/*
 * ControlFlowGraph.cpp
 *
 *  Created on: Jun 6, 2014
 *      Author: luigi
 */
#include "ControlFlowGraph.hpp"
#include "Error.hpp"
#include <sstream>

namespace jnif {

static void addBasicBlock2(InstList::Iterator eit, InstList::Iterator& beginBb,
		int& bbid, ControlFlowGraph& cfg) {
	if (beginBb != eit) {
		std::stringstream ss;
		ss << "BB" << bbid;
		String name = ss.str();

		cfg.addBasicBlock(beginBb, eit, name);

		beginBb = eit;
		bbid++;
	}
}

static void buildBasicBlocks(InstList& instList, ControlFlowGraph& cfg) {
	//setBranchTargets(instList);

	int bbid = 0;
	InstList::Iterator beginBb = instList.begin();

	for (InstList::Iterator it = instList.begin(); it != instList.end(); ++it) {
		Inst* inst = *it;

		if (inst->isLabel()
				&& (inst->label()->isBranchTarget || inst->label()->isTryStart)) {
			addBasicBlock2(it, beginBb, bbid, cfg);
		}

		if (inst->isBranch()) {
			InstList::Iterator eit = it;
			++eit;
			addBasicBlock2(eit, beginBb, bbid, cfg);
		}

		if (inst->isExit()) {
			InstList::Iterator eit = it;
			++eit;
			addBasicBlock2(eit, beginBb, bbid, cfg);
		}
	}
}

static void addTarget2(BasicBlock* bb, Inst* inst, ControlFlowGraph& cfg) {
	Error::assert(inst->isLabel(), "Expected label instruction");
	int labelId = inst->label()->id;
	BasicBlock* tbbid = cfg.findBasicBlockOfLabel(labelId);
	bb->addTarget(tbbid);
}

static void buildCfg(InstList& instList, ControlFlowGraph& cfg) {
	buildBasicBlocks(instList, cfg);

	for (BasicBlock* bb : cfg) {
		if (bb->start == instList.end()) {
			Error::assert(bb->name == "Entry" || bb->name == "Exit", "");
			Error::assert(bb->exit == instList.end(), "");
			continue;
		}

		InstList::Iterator e = bb->exit;
		--e;
		Error::assert(e != instList.end(), "");

		Inst* last = *e;

		if (bb->start == instList.begin()) {
			cfg.entry->addTarget(bb);
		}

		if (last->isJump()) {
			addTarget2(bb, last->jump()->label2, cfg);

			if (last->opcode != OPCODE_goto) {
				Error::assert(bb->next != NULL, "next bb is null");
				bb->addTarget(bb->next);
			}
		} else if (last->isTableSwitch()) {
			addTarget2(bb, last->ts()->def, cfg);

			for (Inst* target : last->ts()->targets) {
				addTarget2(bb, target, cfg);
			}
		} else if (last->isLookupSwitch()) {
			addTarget2(bb, last->ls()->defbyte, cfg);

			for (Inst* target : last->ls()->targets) {
				addTarget2(bb, target, cfg);
			}
		} else if (last->isExit()) {
			bb->addTarget(cfg.exit);
		} else {
			Error::assert(bb->next != NULL, "next bb is null");
			bb->addTarget(bb->next);
		}
	}
}

ControlFlowGraph::ControlFlowGraph(InstList& instList, TypeFactory& typeFactory) :
		entry(addConstBb(instList, "Entry")), exit(
				addConstBb(instList, "Exit")), instList(instList), _typeFactory(
				typeFactory) {
	buildCfg(instList, *this);
}

ControlFlowGraph::~ControlFlowGraph() {
	//cerr << "~cfg";

	for (auto bb : *this) {
		delete bb;
	}
}

BasicBlock* ControlFlowGraph::addBasicBlock(InstList::Iterator start,
		InstList::Iterator end, const String& name) {
	BasicBlock * const bb = new BasicBlock(start, end, name, this,
			_typeFactory);

	if (basicBlocks.size() > 0) {
		BasicBlock* prevbb = basicBlocks.back();
		prevbb->next = bb;
	}

	basicBlocks.push_back(bb);

	return bb;
}

BasicBlock* ControlFlowGraph::findBasicBlockOfLabel(int labelId) const {
	for (BasicBlock* bb : *this) {
		if (bb->start == instList.end()) {
			Error::assert(bb->name == "Entry" || bb->name == "Exit", "");
			Error::assert(bb->exit == instList.end(), "");
			continue;
		}

		Inst* inst = *bb->start;
		if (inst->isLabel() && inst->label()->id == labelId) {
			return bb;
		}
	}

	Error::raise("Invalid label id: ", labelId, " for the instruction list: ",
			", in cfg: ", *this, instList);
}

}
