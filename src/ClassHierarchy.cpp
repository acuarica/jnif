/*
 * ClassHierarchy.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_CLASSHIERARCHY_CPP
#define JNIF_CLASSHIERARCHY_CPP

#include "ClassHierarchy.hpp"
#include "ClassFile.hpp"
#include "Error.hpp"

namespace jnif {

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

bool ClassHierarchy::isAssignableFrom(const String& sub,
		const String& sup) const {

	String cls = sub;
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

}

#endif
