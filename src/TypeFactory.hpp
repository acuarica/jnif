/*
 * TypeFactory.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_TYPEFACTORY_HPP
#define JNIF_TYPEFACTORY_HPP

#include "Type.hpp"
#include <vector>

namespace jnif {

class TypeFactory {
	friend class Type;
public:

	static const Type& topType() {
		return _topType;
	}

	static const Type& intType() {
		return _intType;
	}

	static const Type& floatType() {
		return _floatType;
	}

	static const Type& longType() {
		return _longType;
	}

	static const Type& doubleType() {
		return _doubleType;
	}

	static const Type& booleanType() {
		return _booleanType;
	}

	static const Type& byteType() {
		return _byteType;
	}

	static const Type& charType() {
		return _charType;
	}

	static const Type& shortType() {
		return _shortType;
	}

	static const Type& nullType() {
		return _nullType;
	}

	static const Type& voidType() {
		return _voidType;
	}

	static Type uninitThisType();

	static Type uninitType(short offset, class Inst* label);

	static Type objectType(const String& className, u2 cpindex = 0);

	static Type arrayType(const Type& baseType, u4 dims);

	/**
	 * Parses the const class name.
	 *
	 * @param className the class name to parse.
	 * @returns the type that represents the class name.
	 */
	static Type fromConstClass(const String& className);

	/**
	 * Parses a field descriptor.
	 *
	 * @param fieldDesc the field descriptor to parse.
	 * @returns the type that represents the field descriptor.
	 */
	static Type fromFieldDesc(const char*& fieldDesc);

	/**
	 * Parses a method descriptor.
	 *
	 * @param methodDesc the method descriptor to parse.
	 * @param argsType collection of method arguments of methodDesc.
	 * @returns the type that represents the return type of methodDesc.
	 */
	static Type fromMethodDesc(const char* methodDesc,
			std::vector<Type>* argsType);

private:

	static Type _topType;
	static Type _intType;
	static Type _floatType;
	static Type _longType;
	static Type _doubleType;
	static Type _booleanType;
	static Type _byteType;
	static Type _charType;
	static Type _shortType;
	static Type _nullType;
	static Type _voidType;

	static Type _parseBaseType(const char*& fieldDesc,
			const char* originalFieldDesc);

	static Type _getType(const char*& fieldDesc, const char* originalFieldDesc,
			int dims);

	static Type _getReturnType(const char*& methodDesc);

	static Type _addType(const Type& type);

};

}

#endif
