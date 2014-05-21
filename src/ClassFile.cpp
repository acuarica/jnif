/*
 * Frame.cpp
 *
 *  Created on: May 19, 2014
 *      Author: luigi
 */
#include "jnif.hpp"
#include "jnifex.hpp"

using namespace std;

namespace jnif {

Attrs::~Attrs() {
	for (Attr* attr : attrs) {
		delete attr;
	}
}

CodeAttr::~CodeAttr() {
	if (cfg != nullptr) {
		delete cfg;
	}
}

ClassFile::~ClassFile() {
	for (Field* field : fields) {
		delete field;
	}

	for (Method* method : methods) {
		delete method;
	}
}

Field* ClassFile::addField(ConstIndex nameIndex, ConstIndex descIndex,
		u2 accessFlags) {
	Field* field = new Field(accessFlags, nameIndex, descIndex, this);
	fields.push_back(field);
	return field;
}

Method* ClassFile::addMethod(ConstIndex nameIndex, ConstIndex descIndex,
		u2 accessFlags) {
	Method* method = new Method(accessFlags, nameIndex, descIndex, this);
	methods.push_back(method);
	return method;
}

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
