/*
 * Type.cpp
 *
 *  Created on: Jun 6, 2014
 *      Author: luigi
 */

#include "Type.hpp"
#include "Error.hpp"
#include "TypeFactory.hpp"

namespace jnif {

u2 Type::getCpIndex() const {
	Error::check(isObject(), "Type is not object type to get cp index: ",
			*this);
	return classIndex;
}

String Type::getClassName() const {
	Error::check(isObject(), "Type is not object type to get class name: ",
			*this);
	if (isArray()) {
		std::stringstream ss;
		for (u4 i = 0; i < dims; i++) {
			ss << "[";
		}

		if (tag == TYPE_OBJECT) {
			ss << "L" << className << ";";
		} else {
			ss << className;
		}

		return ss.str();
	} else {
		return className;
	}
}

Type Type::elementType(TypeFactory& typeFactory) const {
	Error::check(isArray(), "Type is not array: ", *this);

	Type type = Type(*this, dims - 1);
	return typeFactory._addType(type);
}

Type Type::stripArrayType(TypeFactory& typeFactory) const {
	Error::check(isArray(), "Type is not array: ", *this);

	Type type = Type(*this, 0);
	return typeFactory._addType(type);
}

long Type::nextTypeId = 2;

}
