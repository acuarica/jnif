/*
 * jnif.cpp
 *
 *  Created on: May 7, 2014
 *      Author: luigi
 */

#include "jnif.hpp"

#include <stdio.h>
#include <execinfo.h>
#include <unistd.h>

using namespace std;

namespace jnif {

bool Method::isInit() const {
	String name = constPool->getUtf8(nameIndex);
	return hasCode() && name == "<init>";
}

bool Method::isMain() const {
	String name = constPool->getUtf8(nameIndex);
	String desc = constPool->getUtf8(descIndex);

	return hasCode() && name == "main" && isStatic() && isPublic()
			&& desc == "([Ljava/lang/String;)V";
}

String Member::getName() const {
	String name = constPool->getUtf8(nameIndex);
	return name;
}

void Error::_backtrace(std::ostream& os) {
	void* array[20];
	size_t size;

	size = backtrace(array, 20);

	char** symbols = backtrace_symbols(array, size);
	for (size_t i = 0; i < size; i++) {
		const char* symbol = symbols[i];
		os << "    " << symbol << endl;
	}

	free(symbols);
}

InstList& Method::instList() {
	for (Attr* attr : attrs) {
		if (attr->kind == ATTR_CODE) {
			return ((CodeAttr*) attr)->instList;
		}
	}

	Error::raise("ERROR! get inst list");
}

Attrs::~Attrs() {
	for (Attr* attr : attrs) {
		delete attr;
	}
}

CodeAttr::~CodeAttr() {
	if (cfg != NULL) {
		delete cfg;
	}
}

static ostream& dotFrame(ostream& os, const Frame& frame) {
//	os << " LVA: ";
//	for (u4 i = 0; i < frame.lva.size(); i++) {
//		os << (i == 0 ? "" : ",\n ") << i << ": " << frame.lva[i];
//	}

	os << " STACK: ";
	int i = 0;
	for (auto t : frame.stack) {
		os << (i == 0 ? "" : "\n  ") << t;
		i++;
	}
	return os << " ";
}

static void dotCfg(ostream& os, const ControlFlowGraph& cfg, int methodId) {

	for (BasicBlock* bb : cfg) {
		os << "    m" << methodId << bb->name << " [ label = \"<port0> "
				<< bb->name;
		os << " |{ ";
		dotFrame(os, bb->in);
		os << " | ";

		for (auto it = bb->start; it != bb->exit; ++it) {
			Inst* inst = *it;
			os << *inst << endl;
			//os << endl;
		}

		os << " | ";
		dotFrame(os, bb->out);
		os << "}\" ]" << endl;

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
		if (method->hasCode() && method->codeAttr()->cfg != NULL) {
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

void Inst::checkCast(bool cond, const char* kindName) const {
	Error::assert(cond, "Inst is not a ", kindName, ": ", *this);
	//Error::assert(cond, "Inst is not a ", kindName,
	//	": <missing instruction due to const>");
}

}
