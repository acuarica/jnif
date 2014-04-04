/*
 * ControlFlowGraph.hpp
 *
 *  Created on: Apr 3, 2014
 *      Author: luigi
 */

#ifndef JNIF_CONTROLFLOWGRAPH_HPP
#define JNIF_CONTROLFLOWGRAPH_HPP

#include <string>
#include <sstream>
#include <tuple>

#include <iostream>

#include "jniferr.hpp"

#include "Graph.hpp"

namespace jnif {

class ControlFlowGraph: public Graph<
		std::tuple<InstList::iterator, InstList::iterator, std::string>> {
public:

	ControlFlowGraph(InstList& instList) :
			entry(
					addNode(
							std::make_tuple(instList.end(), instList.end(),
									"Entry"))), exit(
					addNode(
							std::make_tuple(instList.end(), instList.end(),
									"Exit"))) {
		buildCfg(instList);
	}

	const ControlFlowGraph::NodeKey entry;
	const ControlFlowGraph::NodeKey exit;

private:

	inline static bool isExit(Inst* inst) {
		Opcode opcode = inst->opcode;
		return (opcode >= OPCODE_ireturn && opcode <= OPCODE_return)
				|| opcode == OPCODE_athrow;
	}

	void buildBasicBlocks(InstList& instList) {
		int bbid = 0;
		auto beginBb = instList.begin();

		auto getBasicBlockName = [&](int bbid) {
			std::stringstream ss;
			ss << "BB" << bbid;
			return ss.str();
		};

		auto addBasicBlock = [&](InstList::iterator eit) {
			if (beginBb != eit) {
				string name = getBasicBlockName(bbid);
				auto bb = std::make_tuple(beginBb, eit, name);
				addNode(bb);

				beginBb = eit;
				bbid++;
			}
		};

		for (auto it = instList.begin(); it != instList.end(); it++) {
			Inst* inst = *it;

			if (inst->kind == KIND_LABEL) {
				addBasicBlock(it);
			}

			if (inst->kind == KIND_JUMP) {
				auto eit = it;
				eit++;
				addBasicBlock(eit);
			}

			if (isExit(inst)) {
				auto eit = it;
				eit++;
				addBasicBlock(eit);
			}
		}
	}

	NodeKey findNodeOfLabel(int labelId, InstList& instList) {
		for (auto nid : *this) {
			InstList::iterator b;
			InstList::iterator e;
			std::string name;
			std::tie(b, e, name) = getNode(nid);

			if (b == instList.end()) {
				ASSERT(name == "Entry" || name == "Exit", "");
				ASSERT(e == instList.end(), "");
				continue;
			}

			Inst* inst = *b;
			if (inst->kind == KIND_LABEL && inst->label.id == labelId) {
				return nid;
			}
		}

		throw "se pudrio el chorran";
	}

	void buildCfg(InstList& instList) {

		buildBasicBlocks(instList);

		for (auto nid : *this) {
			InstList::iterator b;
			InstList::iterator e;
			std::string name;
			std::tie(b, e, name) = getNode(nid);

			if (b == instList.end()) {
				ASSERT(name == "Entry" || name == "Exit", "");
				ASSERT(e == instList.end(), "");
				continue;
			}

			std::cerr << name << endl;

			e--;
			ASSERT(e != instList.end(), "");

			//Inst* first = *b;
			Inst* last = *e;

			if (b == instList.begin()) {
				addEdge(entry, nid);
			}

			if (last->kind == KIND_JUMP) {
				int labelId = last->jump.label2->label.id;
				NodeKey tbbid = findNodeOfLabel(labelId, instList);
				addEdge(nid, tbbid);

				if (last->opcode != OPCODE_goto) {
					addEdge(nid, nid.next());
				}
			} else if (isExit(last)) {
				addEdge(nid, exit);
			} else {
				addEdge(nid, nid.next());
			}
		}
	}
};

}

#endif
