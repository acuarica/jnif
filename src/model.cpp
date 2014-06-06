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

ConstIndex ConstPool::getIndexOfUtf8(const char* utf8) {
	auto it = utf8s.find(utf8);
	if (it != utf8s.end()) {
		ConstIndex idx = it->second;
		Error::assert(getUtf8(idx) != utf8, "Error on get index of utf8");
		return idx;
	} else {
		return NULLENTRY;
	}

//		ConstPool& cp = *this;
//		for (ConstPool::Iterator it = cp.iterator(); it.hasNext(); it++) {
//			ConstIndex i = *it;
//			//ConstPool::Tag tag = cp.getTag(i);
//
//			//const Entry* entry = &cp.entries[i];
//
//			if (isUtf8(i) && getUtf8(i) == String(utf8)) {
//				return i;
//			}
//		}
//
//		return NULLENTRY;
}

const ConstItem* ConstPool::_getEntry(ConstIndex i) const {
	Error::check(i > NULLENTRY, "Null access to constant pool: index = ", i);
	Error::check(i < entries.size(), "Index out of bounds: index = ", i);

	const ConstItem* entry = &entries[i];

	return entry;
}

const ConstItem* ConstPool::_getEntry(ConstIndex index, u1 tag,
		const char* message) const {
	const ConstItem* entry = _getEntry(index);

	Error::check(entry->tag == tag, "Invalid constant ", message,
			", expected: ", (int) tag, ", actual: ", (int) entry->tag);

	return entry;
}

InstList& Method::instList() {
	for (Attr* attr : attrs) {
		if (attr->kind == ATTR_CODE) {
			return ((CodeAttr*) attr)->instList;
		}
	}

	Error::raise("ERROR! get inst list");
}

void ClassHierarchy::addClass(const ClassFile& classFile) {
	ClassEntry e;
	e.className = classFile.getThisClassName();

	if (classFile.superClassIndex == ConstPool::NULLENTRY) {
		Error::check(e.className == "java/lang/Object",
				"invalid class name for null super class: ", e.className,
				"asdfasf");
		e.superClassName = "0";
	} else {
		e.superClassName = classFile.getClassName(classFile.superClassIndex);
	}

	//for (ConstIndex interIndex : classFile.interfaces) {
	//const string& interName = classFile.getClassName(interIndex);
	//e.interfaces.push_back(interName);
	//}

	classes[e.className] = e;
	//classes.push_front(e);
}

const String& ClassHierarchy::getSuperClass(const String& className) const {
	auto it = getEntry(className);
	Error::assert(it != classes.end(), "Class not defined");

	return it->second.superClassName;
}

bool ClassHierarchy::isAssignableFrom(const string& sub,
		const string& sup) const {

	string cls = sub;
	while (cls != "0") {
		if (cls == sup) {
			return true;
		}

		cls = getSuperClass(cls);
	}

	return false;
}

bool ClassHierarchy::isDefined(const String& className) const {
//	const ClassHierarchy::ClassEntry* e = getEntry(className);
//	return e != NULL;
	auto it = getEntry(className);
	return it != classes.end();
}

std::map<String, ClassHierarchy::ClassEntry>::const_iterator ClassHierarchy::getEntry(
		const String& className) const {

	auto it = classes.find(className);

	return it;

//	if (it != classes.end()) {
//		return &it->second;
//	} else {
//		return NULL;
//	}
//	for (const ClassHierarchy::ClassEntry& e : *this) {
//		if (e.className == className) {
//			return &e;
//		}
//	}
//
//	return NULL;
}

string Version::supportedByJdk() const {
	if (Version(45, 3) <= *this && *this < Version(45, 0)) {
		return "1.0.2";
	} else if (Version(45, 0) <= *this && *this <= Version(45, 65535)) {
		return "1.1.*";
	} else {
		u2 k = majorVersion - 44;
		stringstream ss;
		ss << "1." << k;
		return ss.str();
	}
}

std::ostream& operator<<(std::ostream& os, const ClassHierarchy&) {
//	for (const ClassHierarchy::ClassEntry& e : ch) {
//		os << "Class: " << e.className << ", ";
//		os << "Super: " << e.superClassName << ", ";
//		os << "Interfaces: { ";
//		for (const string& interName : e.interfaces) {
//			os << interName << " ";
//		}
//
//		os << " }" << endl;
//	}

	return os;
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
