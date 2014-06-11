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

	~TypeFactory();

	const Type& topType() {
		return _topType;
	}

	const Type& intType() {
		return _intType;
	}

	const Type& floatType() {
		return _floatType;
	}

	const Type& longType() {
		return _longType;
	}

	const Type& doubleType() {
		return _doubleType;
	}

	const Type& booleanType() {
		return _booleanType;
	}

	const Type& byteType() {
		return _byteType;
	}

	const Type& charType() {
		return _charType;
	}

	const Type& shortType() {
		return _shortType;
	}

	const Type& nullType() {
		return _nullType;
	}

	const Type& voidType() {
		return _voidType;
	}

	Type uninitThisType();

	Type uninitType(short offset, class Inst* label);

	Type objectType(const String& className, u2 cpindex = 0);

	Type arrayType(const Type& baseType, u4 dims);

	/**
	 * Parses the const class name.
	 *
	 * @param className the class name to parse.
	 * @returns the type that represents the class name.
	 */
	Type fromConstClass(const String& className);

	/**
	 * Parses a field descriptor.
	 *
	 * @param fieldDesc the field descriptor to parse.
	 * @returns the type that represents the field descriptor.
	 */
	Type fromFieldDesc(const char*& fieldDesc);

	/**
	 * Parses a method descriptor.
	 *
	 * @param methodDesc the method descriptor to parse.
	 * @param argsType collection of method arguments of methodDesc.
	 * @returns the type that represents the return type of methodDesc.
	 */
	Type fromMethodDesc(const char* methodDesc, std::vector<Type>* argsType);

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

	Type _parseBaseType(const char*& fieldDesc, const char* originalFieldDesc);

	Type _getType(const char*& fieldDesc, const char* originalFieldDesc,
			int dims);

	Type _getReturnType(const char*& methodDesc);

	Type _addType(const Type& type);

	//std::list<Type*> _typePool;

};

}

#endif
