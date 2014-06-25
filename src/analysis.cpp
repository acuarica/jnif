/*
 * analysis.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: luigi
 */
#include "jnif.hpp"
#include "analysis/SmtBuilder.hpp"
#include "analysis/ComputeFrames.hpp"
#include "analysis/FrameGenerator.hpp"

#include <iostream>

using namespace std;

namespace jnif {

std::ostream& operator<<(std::ostream& os, const Method* m) {
	if (m == NULL) {
		return os << "null method";
	} else {
		return os << *m;
	}
}

void Frame::join(Frame& how, IClassPath* classPath) {
	TypeFactory typeFactory;
	ComputeFrames comp;
	comp.join(typeFactory, *this, how, classPath);
}

void ClassFile::computeFrames(IClassPath* classPath) {
	computeSize();

	FrameGenerator fg(*this, classPath);

	for (Method* method : methods) {
		CodeAttr* code = method->codeAttr();

		if (code != NULL) {
			bool hasJsrOrRet = code->instList.hasJsrOrRet();
			if (hasJsrOrRet) {
				return;
			}

			fg.computeFrames(code, method);
		}
	}
}

}
