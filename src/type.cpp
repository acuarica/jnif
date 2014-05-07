/*
 * type.cpp
 *
 *  Created on: May 7, 2014
 *      Author: luigi
 */
#include "jnif.hpp"

using namespace std;

namespace jnif {

Type Type::fromConstClass(const std::string& className) {
	Error::assert(!className.empty(), "Invalid string class");

	if (className[0] == '[') {
		const char* classNamePtr = className.c_str();
		Type arrayType = fromFieldDesc(classNamePtr);
		Error::assert(arrayType.isArray(), "Not an array: ", arrayType);
		return arrayType;
	} else {
		return Type::objectType(className);
	}
}

Type Type::fromFieldDesc(const char*& fieldDesc) {
	const char* originalFieldDesc = fieldDesc;

	int dims = 0;
	while (*fieldDesc == '[') {
		Error::check(*fieldDesc != '\0',
				"Reach end of string while searching for array. Field descriptor: ",
				originalFieldDesc);
		fieldDesc++;
		dims++;
	}

	Error::check(*fieldDesc != '\0', "");

	auto parseBaseType = [&] () {
		switch (*fieldDesc) {
			case 'Z':
			return Type::booleanType();
			case 'B':
			return Type::byteType();
			case 'C':
			return Type::charType();
			case 'S':
			return Type::shortType();
			case 'I':
			return Type::intType();
			case 'D':
			return Type::doubleType();
			case 'F':
			return Type::floatType();
			case 'J':
			return Type::longType();
			case 'L': {
				fieldDesc++;

				const char* classNameStart = fieldDesc;
				int len = 0;
				while (*fieldDesc != ';') {
					Error::check(*fieldDesc != '\0', "");
					fieldDesc++;
					len++;
				}

				string className (classNameStart, len);
				return Type::objectType(className);
			}
			default:
			Error::raise("Invalid field desc ", originalFieldDesc);
		}};

	Type t = [&]() {
		Type baseType = parseBaseType();
		if (dims == 0) {
			return baseType;
		} else {
			return Type::arrayType(baseType, dims);
		}
	}();

	fieldDesc++;

	return t;
}

Type Type::fromMethodDesc(const char* methodDesc, vector<Type>* argsType) {
	const char* originalMethodDesc = methodDesc;

	Error::check(*methodDesc == '(', "Invalid beginning of method descriptor: ",
			originalMethodDesc);
	methodDesc++;

	while (*methodDesc != ')') {
		Error::check(*methodDesc != '\0', "Reached end of string: ",
				originalMethodDesc);

		Type t = fromFieldDesc(methodDesc);
		argsType->push_back(t);
	}

	Error::check(*methodDesc == ')', "Expected ')' in method descriptor: ",
			originalMethodDesc);
	methodDesc++;

	Error::check(*methodDesc != '\0', "Reached end of string: ",
			originalMethodDesc);

	Type returnType = [&]() {
		if (*methodDesc == 'V') {
			methodDesc++;
			return Type::voidType();
		} else {
			return fromFieldDesc(methodDesc);
		}
	}();

	Error::check(*methodDesc == '\0', "Expected end of string: %s",
			originalMethodDesc);

	return returnType;
}

}

