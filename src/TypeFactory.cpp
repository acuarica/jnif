/*
 * TypeFactory.cpp
 *
 *  Created on: Jun 6, 2014
 *      Author: luigi
 */

#include "TypeFactory.hpp"
#include "Error.hpp"

namespace jnif {

Type TypeFactory::uninitThisType() {
	Type type = Type(TYPE_UNINITTHIS);

	return _addType(type);
}

Type TypeFactory::uninitType(short offset, class Inst* label) {
	Type type = Type(TYPE_UNINIT, offset, label);
	return _addType(type);
}

Type TypeFactory::objectType(const String& className, u2 cpindex) {
	Error::check(!className.empty(),
			"Expected non-empty class name for object type");

	Type type = Type(TYPE_OBJECT, className, cpindex);
	return _addType(type);
}

Type TypeFactory::arrayType(const Type& baseType, u4 dims) {
	//u4 d = baseType.dims + dims;
	Error::check(dims > 0, "Invalid dims: ", dims);
	Error::check(dims <= 255, "Invalid dims: ", dims);
	Error::check(!baseType.isTop(), "Cannot construct an array type of ", dims,
			" dimension(s) using as a base type Top (", baseType, ")");
//		Error::check(!baseType.isArray(), "base type is already an array: ",
//				baseType);

	Type type = Type(baseType, dims);
	return _addType(type);
}

Type TypeFactory::fromConstClass(const String& className) {
	Error::assert(!className.empty(), "Invalid string class");

	if (className[0] == '[') {
		const char* classNamePtr = className.c_str();
		const Type& arrayType = fromFieldDesc(classNamePtr);
		Error::assert(arrayType.isArray(), "Not an array: ", arrayType);
		return arrayType;
	} else {
		return objectType(className);
	}
}

Type TypeFactory::_parseBaseType(const char*& fieldDesc,
		const char* originalFieldDesc) {
	switch (*fieldDesc) {
		case 'Z':
			return booleanType();
		case 'B':
			return byteType();
		case 'C':
			return charType();
		case 'S':
			return shortType();
		case 'I':
			return intType();
		case 'D':
			return doubleType();
		case 'F':
			return floatType();
		case 'J':
			return longType();
		case 'L': {
			fieldDesc++;

			const char* classNameStart = fieldDesc;
			int len = 0;
			while (*fieldDesc != ';') {
				Error::check(*fieldDesc != '\0', "");
				fieldDesc++;
				len++;
			}

			String className(classNameStart, len);
			return objectType(className);
		}
		default:
			Error::raise("Invalid field desc ", originalFieldDesc);
	}
}

Type TypeFactory::_getType(const char*& fieldDesc,
		const char* originalFieldDesc, int dims) {
	const Type& baseType = _parseBaseType(fieldDesc, originalFieldDesc);
	if (dims == 0) {
		return baseType;
	} else {
		return arrayType(baseType, dims);
	}
}

Type TypeFactory::fromFieldDesc(const char*& fieldDesc) {
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

	const Type& t = _getType(fieldDesc, originalFieldDesc, dims);

	fieldDesc++;

	return t;
}

Type TypeFactory::_getReturnType(const char*& methodDesc) {
	if (*methodDesc == 'V') {
		methodDesc++;
		return voidType();
	} else {
		return fromFieldDesc(methodDesc);
	}
}

Type TypeFactory::fromMethodDesc(const char* methodDesc,
		std::vector<Type>* argsType) {
	const char* originalMethodDesc = methodDesc;

	Error::check(*methodDesc == '(', "Invalid beginning of method descriptor: ",
			originalMethodDesc);
	methodDesc++;

	while (*methodDesc != ')') {
		Error::check(*methodDesc != '\0', "Reached end of string: ",
				originalMethodDesc);

		const Type& t = fromFieldDesc(methodDesc);
		argsType->push_back(t);
	}

	Error::check(*methodDesc == ')', "Expected ')' in method descriptor: ",
			originalMethodDesc);
	methodDesc++;

	Error::check(*methodDesc != '\0', "Reached end of string: ",
			originalMethodDesc);

	const Type& returnType = _getReturnType(methodDesc);

	Error::check(*methodDesc == '\0', "Expected end of string: %s",
			originalMethodDesc);

	return returnType;
}

Type TypeFactory::_addType(const Type& type) {
	//_typePool.push_back(type);
	return type;
}

Type TypeFactory::_topType(TYPE_TOP);
Type TypeFactory::_intType(TYPE_INTEGER, "I");
Type TypeFactory::_floatType(TYPE_FLOAT, "F");
Type TypeFactory::_longType(TYPE_LONG, "J");
Type TypeFactory::_doubleType(TYPE_DOUBLE, "D");
Type TypeFactory::_booleanType(TYPE_BOOLEAN, "Z");
Type TypeFactory::_byteType(TYPE_BYTE, "B");
Type TypeFactory::_charType(TYPE_CHAR, "C");
Type TypeFactory::_shortType(TYPE_SHORT, "S");
Type TypeFactory::_nullType(TYPE_NULL);
Type TypeFactory::_voidType(TYPE_VOID);

}
