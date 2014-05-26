/*
 * type.cpp
 *
 *  Created on: May 7, 2014
 *      Author: luigi
 */
#include "jnif.hpp"
#include "jnifex.hpp"

#include <sstream>

using namespace std;

namespace jnif {

Type Type::objectType(const String& className, u2 cpindex) {
	Error::check(!className.empty(),
			"Expected non-empty class name for object type");

	return Type(TYPE_OBJECT, className, cpindex);
}

Type Type::arrayType(const Type& baseType, u4 dims) {
	//u4 d = baseType.dims + dims;
	Error::check(dims > 0, "Invalid dims: ", dims);
	Error::check(dims <= 255, "Invalid dims: ", dims);
	Error::check(!baseType.isTop(), "Cannot construct an array type of ", dims,
			" dimension(s) using as a base type Top (", baseType, ")");
//		Error::check(!baseType.isArray(), "base type is already an array: ",
//				baseType);

	return Type(baseType, dims);
}

String Type::getClassName() const {
	Error::check(isObject(), "Type is not object type to get class name: ",
			*this);

	if (isArray()) {
		stringstream ss;
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

u2 Type::getCpIndex() const {
	Error::check(isObject(), "Type is not object type to get cp index: ",
			*this);
	return classIndex;
}

Type Type::elementType() const {
	Error::check(isArray(), "Type is not array: ", *this);

	return Type(*this, dims - 1);
}

Type Type::stripArrayType() const {
	Error::check(isArray(), "Type is not array: ", *this);

	return Type(*this, 0);
}

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

	auto parseBaseType = [&] () -> Type {
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

Type Type::_topType(TYPE_TOP);
Type Type::_intType(TYPE_INTEGER, "I");
Type Type::_floatType(TYPE_FLOAT, "F");
Type Type::_longType(TYPE_LONG, "J");
Type Type::_doubleType(TYPE_DOUBLE, "D");
Type Type::_booleanType(TYPE_BOOLEAN, "Z");
Type Type::_byteType(TYPE_BYTE, "B");
Type Type::_charType(TYPE_CHAR, "C");
Type Type::_shortType(TYPE_SHORT, "S");
Type Type::_nullType(TYPE_NULL);
Type Type::_uninitThisType(TYPE_UNINITTHIS);

}

