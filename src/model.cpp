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
	for (Attr* attr : attrs) {
		delete attr;
	}
}

CodeAttr::~CodeAttr() {
	if (cfg != NULL) {
		delete cfg;
	}
}

}
