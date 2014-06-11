/*
 * Type.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_TYPE_HPP
#define JNIF_TYPE_HPP

#include "base.hpp"

namespace jnif {

/**
 *
 */
enum TypeTag {
	TYPE_TOP = 0,
	TYPE_INTEGER = 1,
	TYPE_FLOAT = 2,
	TYPE_LONG = 4,
	TYPE_DOUBLE = 3,
	TYPE_NULL = 5,
	TYPE_UNINITTHIS = 6,
	TYPE_OBJECT = 7,
	TYPE_UNINIT = 8,
	TYPE_VOID,
	TYPE_BOOLEAN,
	TYPE_BYTE,
	TYPE_CHAR,
	TYPE_SHORT,
};

/**
 * Verification type class
 */
class Type {
	friend class TypeFactory;

public:

	bool operator==(const Type& other) const {
		return tag == other.tag
				&& (tag != TYPE_OBJECT || className == other.className)
				&& dims == other.dims;
	}

	friend bool operator!=(const Type& lhs, const Type& rhs) {
		return !(lhs == rhs);
	}

	bool isTop() const {
		return tag == TYPE_TOP && !isArray();
	}

	/**
	 * Returns true when this type is exactly int type.
	 * False otherwise.
	 */
	bool isInt() const {
		return tag == TYPE_INTEGER && !isArray();
	}

	/**
	 * Returns true when this type is an
	 * integral type, i.e., int, boolean, byte, char, short.
	 * False otherwise.
	 */
	bool isIntegral() const {
		switch (tag) {
			case TYPE_INTEGER:
			case TYPE_BOOLEAN:
			case TYPE_BYTE:
			case TYPE_CHAR:
			case TYPE_SHORT:
				return !isArray();
			default:
				return false;
		}
	}

	bool isFloat() const {
		return tag == TYPE_FLOAT && !isArray();
	}

	bool isLong() const {
		return tag == TYPE_LONG && !isArray();
	}

	bool isDouble() const {
		return tag == TYPE_DOUBLE && !isArray();
	}

	bool isNull() const {
		return tag == TYPE_NULL;
	}

	bool isUninitThis() const {
		return tag == TYPE_UNINITTHIS;
	}

	bool isUninit() const {
		return tag == TYPE_UNINIT;
	}

	bool isObject() const {
		return tag == TYPE_OBJECT || isArray();
	}

	bool isArray() const {
		return dims > 0;
	}

	bool isVoid() const {
		return tag == TYPE_VOID;
	}

	bool isOneWord() const {
		return isIntegral() || isFloat() || isNull() || isObject()
				|| isUninitThis();
	}

	bool isTwoWord() const {
		return isLong() || isDouble();
	}

	bool isOneOrTwoWord() const {
		return isOneWord() || isTwoWord();
	}

	bool isClass() const {
		return isObject() && !isArray();
	}

	String getClassName() const;

	u2 getCpIndex() const;

	void setCpIndex(u2 index) {
		//check(isObject(), "Type is not object type to get cp index: ", *this);
		classIndex = index;
	}

	u4 getDims() const {
		return dims;
	}

	/**
	 * Returns the element type of this array type. Requires that this type
	 * is an array type. The return type is the same base type but with a less
	 * dimension.
	 *
	 * For example, assuming that this type represents [[[I, then the result
	 * value is [[I.
	 *
	 * @returns the element type of this array.
	 */
	Type elementType(class TypeFactory& typeFactory) const;

	/**
	 * Removes the any dimension on this type. This type has to be an array
	 * type.
	 *
	 * For example, assuming that this type represents [[[I, then the result
	 * value is I.
	 *
	 * @returns the base type of this type. The result ensures that is not an
	 * array type.
	 */
	Type stripArrayType(class TypeFactory& typeFactory) const;

	union {
		mutable struct {
			short offset;
			class Inst* label;
			class TypeInst* newinst;
		} uninit;
	};

	mutable bool init;

	mutable long typeId;

	static long nextTypeId;

	TypeTag tag;
	u4 dims;
	u2 classIndex;
	String className;

private:

	Type(TypeTag tag) :
			init(true), typeId(0), tag(tag), dims(0), classIndex(0) {
	}

	Type(TypeTag tag, short offset, Inst* label) :
			init(true), typeId(0), tag(tag), dims(0), classIndex(0) {
		uninit.offset = offset;
		uninit.label = label;
	}

	Type(TypeTag tag, const String& className, u2 classIndex = 0) :
			init(true), typeId(0), tag(tag), dims(0), classIndex(classIndex), className(
					className) {
	}

	Type(const Type& other, u4 dims) {
		this->dims = dims;

		// Type(other)
		uninit.offset = other.uninit.offset;
		uninit.label = other.uninit.label;
		uninit.newinst = other.uninit.newinst;

		init = other.init;
		typeId = other.typeId;
		tag = other.tag;
		classIndex = other.classIndex;
		className = other.className;
	}

};

std::ostream& operator<<(std::ostream& os, const Type& type);

}

#endif
