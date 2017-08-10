/*
 * jnif.cpp
 *
 *  Created on: May 7, 2014
 *      Author: luigi
 */

#include "jnif.hpp"

using namespace std;

namespace jnif {

Attrs::~Attrs() {
  Error::trace("~Attrs");

	for (Attr* attr : attrs) {
		attr->~Attr();
	}
}

CodeAttr::~CodeAttr() {
  Error::trace("~CodeAttr");

	if (cfg != NULL) {
		delete cfg;
	}
}

const char* SourceFileAttr::sourceFile() const {
  return constPool->getUtf8(sourceFileIndex);
}

const char* SignatureAttr::signature() const {
  return constPool->getUtf8(signatureIndex);
}

}
