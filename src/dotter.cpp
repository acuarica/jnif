/*
 * dotter.cpp
 *
 *  Created on: May 7, 2014
 *      Author: luigi
 */

#include "jnif.hpp"

using namespace std;

namespace jnif {

static ostream& dotFrame(ostream& os, const Frame& frame) {
	os << " LVA: ";
	for (u4 i = 0; i < frame.lva.size(); i++) {
		os << (i == 0 ? "" : ", ") << i << ": " << frame.lva[i];
	}

	os << " STACK: ";
	int i = 0;
	for (auto t : frame.stack) {
		os << (i == 0 ? "" : "  ") << t;
		i++;
	}
	return os << " ";
}

static void dotCfg(ostream& os, const ControlFlowGraph& cfg, int methodId) {

	for (BasicBlock* bb : cfg) {
		os << "    m" << methodId << bb->name << " [ label = \"<port0> "
				<< bb->name;
		os << " | ";
		dotFrame(os, bb->in);
		os << " | ";
		dotFrame(os, bb->out);
		os << "\" ]" << endl;

//		for (auto it = bb->start; it != bb->exit; it++) {
//			Inst* inst = *it;
//			printInst(*inst);
//			os << endl;
//		}
	}

	for (BasicBlock* bb : cfg) {
		for (BasicBlock* bbt : *bb) {
			os << "    m" << methodId << bb->name << " -> m" << methodId
					<< bbt->name << "" << endl;
		}
	}
}

void ClassFile::dot(ostream& os) const {
	os << "digraph Cfg {" << endl;
	os << "  graph [ label=\"Class " << getThisClassName() << "\" ]" << endl;
	os << "  node [ shape = \"record\" ]" << endl;

	int methodId = 0;
	for (const Method* method : methods) {
		if (method->hasCode() && method->codeAttr()->cfg != nullptr) {
			const string& methodName = getUtf8(method->nameIndex);
			const string& methodDesc = getUtf8(method->descIndex);

			os << "  subgraph method" << methodId << "{" << endl;
			os << "    graph [bgcolor=gray90, label=\"Method " << methodName
					<< methodDesc << "\"]" << endl;
			dotCfg(os, *method->codeAttr()->cfg, methodId);

			os << "  }" << endl;

			methodId++;
		}
	}

	os << "}" << endl;
}

}
