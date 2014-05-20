/*
 * Frame.cpp
 *
 *  Created on: May 19, 2014
 *      Author: luigi
 */
#include "jnif.hpp"
#include "jnifex.hpp"

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

}
