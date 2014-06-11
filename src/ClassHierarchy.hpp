/*
 * ClassHierarchy.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_CLASSHIERARCHY_HPP
#define JNIF_CLASSHIERARCHY_HPP

#include "base.hpp"
#include <map>

namespace jnif {

class ClassFile;

/**
 *
 */
class ClassHierarchy {
public:

//	IClassFinder* finder;
//
//	ClassHierarchy(IClassFinder* finder) :
//			finder(finder) {
//	}

	/**
	 *
	 */
	class ClassEntry {
	public:
		String className;
		String superClassName;
		//std::vector<String> interfaces;
	};

	/**
	 *
	 */
	//ClassFile* defineClass(const ClassFile& classFile);
	/**
	 *
	 */
	void addClass(const ClassFile& classFile);

	const String& getSuperClass(const String& className) const;

	bool isAssignableFrom(const String& sub, const String& sup) const;

	bool isDefined(const String& className) const;

//	std::list<ClassEntry>::iterator begin() {
//		return classes.begin();
//	}
//
//	std::list<ClassEntry>::iterator end() {
//		return classes.end();
//	}
//
//	std::list<ClassEntry>::const_iterator begin() const {
//		return classes.begin();
//	}
//
//	std::list<ClassEntry>::const_iterator end() const {
//		return classes.end();
//	}

	friend std::ostream& operator<<(std::ostream& os, const ClassHierarchy& ch);

private:

	//std::list<ClassEntry> classes;

	std::map<String, ClassEntry> classes;

	std::map<String, ClassEntry>::const_iterator getEntry(
			const String& className) const;
};

}

#endif
