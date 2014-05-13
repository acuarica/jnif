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

namespace jnif {

void Error::_backtrace() {
	void *array[20];
	size_t size;

	size = backtrace(array, 20);

	fprintf(stderr, "Error: exception on jnif: (backtrace)\n");
	backtrace_symbols_fd(array, size, STDERR_FILENO);
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

	classes.push_front(e);
}

const string& ClassHierarchy::getSuperClass(const string& className) const {
	const ClassHierarchy::ClassEntry* e = getEntry(className);
	return e->superClassName;
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
	const ClassHierarchy::ClassEntry* e = getEntry(className);
	return e != nullptr;
}

const ClassHierarchy::ClassEntry* ClassHierarchy::getEntry(
		const string& className) const {
	for (const ClassHierarchy::ClassEntry& e : *this) {
		if (e.className == className) {
			return &e;
		}
	}

	return nullptr;
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
	for (const ClassHierarchy::ClassEntry& e : ch) {
		os << "Class: " << e.className << ", ";
		os << "Super: " << e.superClassName << ", ";
		os << "Interfaces: { ";
		for (const string& interName : e.interfaces) {
			os << interName << " ";
		}

		os << " }" << endl;
	}

	return os;
}

}
