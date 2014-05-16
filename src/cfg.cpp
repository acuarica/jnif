/*
 * cfg.cpp
 *
 *  Created on: May 7, 2014
 *      Author: luigi
 */

#include "jnif.hpp"

using namespace std;

namespace jnif {

static void setBranchTargets(InstList& instList) {
	for (Inst* inst : instList) {
		if (inst->isJump()) {
			inst->jump()->label2->label()->isBranchTarget = true;
		} else if (inst->isTableSwitch()) {
			inst->ts()->def->label()->isBranchTarget = true;
			for (Inst* target : inst->ts()->targets) {
				target->label()->isBranchTarget = true;
			}
		} else if (inst->isLookupSwitch()) {
			inst->ls()->defbyte->label()->isBranchTarget = true;
			for (Inst* target : inst->ls()->targets) {
				target->label()->isBranchTarget = true;
			}
		}
	}
}

static void buildBasicBlocks(InstList& instList, ControlFlowGraph& cfg) {
	setBranchTargets(instList);

	int bbid = 0;
	auto beginBb = instList.begin();

	auto getBasicBlockName = [&](int bbid) {
		stringstream ss;
		ss << "BB" << bbid;

		return ss.str();
	};

	auto addBasicBlock2 = [&](InstList::Iterator eit) {
		if (beginBb != eit) {
			string name = getBasicBlockName(bbid);
			cfg.addBasicBlock(beginBb, eit, name);

			beginBb = eit;
			bbid++;
		}
	};

	for (auto it = instList.begin(); it != instList.end(); ++it) {
		Inst* inst = *it;

		if (inst->isLabel()
				&& (inst->label()->isBranchTarget || inst->label()->isTryStart)) {
			addBasicBlock2(it);
		}

		if (inst->isBranch()) {
			auto eit = it;
			++eit;
			addBasicBlock2(eit);
		}

		if (inst->isExit()) {
			auto eit = it;
			++eit;
			addBasicBlock2(eit);
		}
	}
}

static void buildCfg(InstList& instList, ControlFlowGraph& cfg) {
	buildBasicBlocks(instList, cfg);

	auto addTarget2 = [&] (BasicBlock* bb, Inst* inst) {
		Error::assert(inst->isLabel(), "Expected label instruction");
		int labelId = inst->label()->id;
		BasicBlock* tbbid = cfg.findBasicBlockOfLabel(labelId);
		bb->addTarget(tbbid);
	};

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
			addTarget2(bb, last->jump()->label2);

			if (last->opcode != OPCODE_goto) {
				Error::assert(bb->next != nullptr, "next bb is null");
				bb->addTarget(bb->next);
			}
		} else if (last->isTableSwitch()) {
			addTarget2(bb, last->ts()->def);

			for (Inst* target : last->ts()->targets) {
				addTarget2(bb, target);
			}
		} else if (last->isLookupSwitch()) {
			addTarget2(bb, last->ls()->defbyte);

			for (Inst* target : last->ls()->targets) {
				addTarget2(bb, target);
			}
		} else if (last->isExit()) {
			bb->addTarget(cfg.exit);
		} else {
			Error::assert(bb->next != nullptr, "next bb is null");
			bb->addTarget(bb->next);
		}
	}
}

ControlFlowGraph::ControlFlowGraph(InstList& instList) :
		entry(addConstBb(instList, "Entry")), exit(
				addConstBb(instList, "Exit")), instList(instList) {
	buildCfg(instList, *this);
}

BasicBlock* ControlFlowGraph::addBasicBlock(InstList::Iterator start,
		InstList::Iterator end, const String& name) {
	BasicBlock * const bb = new BasicBlock(start, end, name, this);

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

	InstList& il = (InstList&) instList;

	Error::raise("Invalid label id: ", labelId, " for the instruction list: ",
			", in cfg: ", *this);
}

std::ostream& operator<<(std::ostream& os, BasicBlock& bb) {
	os << "* " << bb.name;

	os << " @Out { ";
	for (BasicBlock* bbt : bb) {
		os << "->" << bbt->name << ", ";
	}
	os << "} ";

	os << "in frame: " << bb.in << ", out frame: " << bb.out;
	os << endl;

	for (auto it = bb.start; it != bb.exit; ++it) {
		Inst* inst = *it;
		os << *inst << endl;
	}

	return os;
}

std::ostream& operator<<(std::ostream& os, const ControlFlowGraph& cfg) {
	for (BasicBlock* bb : cfg) {
		os << *bb;
	}

	os << endl;

	return os;
}

}
