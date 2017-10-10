/*
 * jnif.cpp
 *
 *  Created on: May 7, 2014
 *      Author: luigi
 */

#include "Attr.hpp"

#include "SignatureAttr.hpp"

#include "../ClassFile.hpp"

using namespace std;


namespace jnif::model {

Attrs::~Attrs() {
	for (Attr* attr : attrs) {
		attr->~Attr();
	}
}

CodeAttr::~CodeAttr() {
	if (cfg != nullptr) {
		delete cfg;
	}
}

const char* SignatureAttr::signature() const {
  return constPool->getUtf8(signatureIndex);
}

}
