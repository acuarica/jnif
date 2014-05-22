/*
 * jnif.cpp
 *
 *  Created on: May 7, 2014
 *      Author: luigi
 */

#include "jnif.hpp"
#include "jnifex.hpp"

#include <stdio.h>
#include <execinfo.h>
#include <unistd.h>
#include <sstream>

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

std::ostream& operator<<(std::ostream& os, const JnifException& ex) {
	os << "Error: JNIF Exception: " << ex.message << " @ " << endl;
	os << ex.stackTrace;

	return os;

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
		Error::check(e.className == "java/lang/Object", "invalid super class");
		e.superClassName = "0";
	} else {
		e.superClassName = classFile.getClassName(classFile.superClassIndex);
	}

	for (ConstIndex interIndex : classFile.interfaces) {
		const string& interName = classFile.getClassName(interIndex);
		e.interfaces.push_back(interName);
	}

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
//	return e != nullptr;
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
//		return nullptr;
//	}
//	for (const ClassHierarchy::ClassEntry& e : *this) {
//		if (e.className == className) {
//			return &e;
//		}
//	}
//
//	return nullptr;
}

string Version::supportedByJdk() const {
	if (Version(45, 3) <= *this && *this < Version(45, 0)) {
		return "1.0.2";
	} else if (Version(45, 0) <= *this && *this <= Version(45, 65535)) {
		return "1.1.*";
	} else {
		u2 k = major - 44;
		stringstream ss;
		ss << "1." << k;
		return ss.str();
	}
}

std::ostream& operator<<(std::ostream& os, const ClassHierarchy& ch) {
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

}
