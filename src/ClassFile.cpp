/*
 * ClassFile.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: luigi
 */

#include "ClassFile.hpp"

namespace jnif {

ClassFile::~ClassFile() {
//	for (Field* field : fields) {
//		delete field;
//	}
//
//	for (Method* method : methods) {
//		delete method;
//	}
}

Field* ClassFile::addField(ConstIndex nameIndex, ConstIndex descIndex,
		u2 accessFlags) {
	void* buf = _arena.alloc(sizeof(Field));
	Field* field = new (buf) Field(accessFlags, nameIndex, descIndex, this);
	fields.push_back(field);
	return field;
}

Method* ClassFile::addMethod(ConstIndex nameIndex, ConstIndex descIndex,
		u2 accessFlags) {
	void* buf = _arena.alloc(sizeof(Method));
	Method* method = new (buf) Method(accessFlags, nameIndex, descIndex, this);
	methods.push_back(method);
	return method;
}

}

