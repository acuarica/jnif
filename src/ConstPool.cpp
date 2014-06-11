/*
 * ConstPool.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: luigi
 */
#include "ConstPool.hpp"
#include "Error.hpp"

namespace jnif {

u4 ConstPool::size() const {
	return entries.size();
}

ConstPool::Iterator ConstPool::iterator() const {
	return Iterator(*this, 1);
}

ConstIndex ConstPool::addClass(ConstIndex classNameIndex) {
	ConstItem e(CONST_CLASS);
	e.clazz.nameIndex = classNameIndex;

	return _addSingle(e);
}

ConstIndex ConstPool::addClass(const char* className) {
	ConstIndex classNameIndex = addUtf8(className);
	return addClass(classNameIndex);
}

ConstIndex ConstPool::addFieldRef(ConstIndex classIndex,
		ConstIndex nameAndTypeIndex) {
	ConstItem e(CONST_FIELDREF, classIndex, nameAndTypeIndex);
	return _addSingle(e);
}

ConstIndex ConstPool::addMethodRef(ConstIndex classIndex,
		ConstIndex nameAndTypeIndex) {
	ConstItem e(CONST_METHODREF, classIndex, nameAndTypeIndex);
	return _addSingle(e);
}

ConstIndex ConstPool::addMethodRef(ConstIndex classIndex, const char* name,
		const char* desc) {
	ConstIndex methodNameIndex = addUtf8(name);
	ConstIndex methodDescIndex = addUtf8(desc);
	ConstIndex nameAndTypeIndex = addNameAndType(methodNameIndex,
			methodDescIndex);
	ConstIndex methodRefIndex = addMethodRef(classIndex, nameAndTypeIndex);

	return methodRefIndex;
}

ConstIndex ConstPool::addInterMethodRef(ConstIndex classIndex,
		ConstIndex nameAndTypeIndex) {
	ConstItem e(CONST_INTERMETHODREF, classIndex, nameAndTypeIndex);
	return _addSingle(e);
}

ConstIndex ConstPool::addString(ConstIndex utf8Index) {
	ConstItem e(CONST_STRING);
	e.s.stringIndex = utf8Index;

	return _addSingle(e);
}

ConstIndex ConstPool::addString(const String& str) {
	ConstIndex utf8Index = addUtf8(str.c_str());
	return addString(utf8Index);
}

ConstIndex ConstPool::addStringFromClass(ConstIndex classIndex) {
	ConstIndex classNameIndex = getClassNameIndex(classIndex);
	ConstIndex classNameStringIndex = addString(classNameIndex);

	return classNameStringIndex;
}

ConstIndex ConstPool::addInteger(int value) {
	ConstItem e(CONST_INTEGER, value);
	return _addSingle(e);
}

ConstIndex ConstPool::addFloat(float value) {
	ConstItem e(CONST_FLOAT, value);
	return _addSingle(e);
}

ConstIndex ConstPool::addLong(long value) {
	ConstItem e(CONST_LONG, value);
	return _addDoubleEntry(e);
}

ConstIndex ConstPool::addDouble(double value) {
	ConstItem entry(CONST_DOUBLE, value);
	return _addDoubleEntry(entry);
}

ConstIndex ConstPool::addNameAndType(ConstIndex nameIndex,
		ConstIndex descIndex) {
	ConstItem e(CONST_NAMEANDTYPE);
	e.nameandtype.nameIndex = nameIndex;
	e.nameandtype.descriptorIndex = descIndex;

	return _addSingle(e);
}

ConstIndex ConstPool::addUtf8(const char* utf8, int len) {
	String str(utf8, len);
	ConstItem e(CONST_UTF8, str);

	ConstIndex i = _addSingle(e);
	utf8s[String(str)] = i;

	return i;
}

ConstIndex ConstPool::addUtf8(const char* str) {
	ConstItem e(CONST_UTF8, str);
	ConstIndex i = _addSingle(e);
	utf8s[String(str)] = i;

	return i;
}

ConstIndex ConstPool::addMethodHandle(u1 refKind, u2 refIndex) {
	ConstItem e(CONST_METHODHANDLE);
	e.methodhandle.referenceKind = refKind;
	e.methodhandle.referenceIndex = refIndex;
	return _addSingle(e);
}

ConstIndex ConstPool::addMethodType(u2 descIndex) {
	ConstItem e(CONST_METHODTYPE);
	e.methodtype.descriptorIndex = descIndex;
	return _addSingle(e);
}

ConstIndex ConstPool::addInvokeDynamic(u2 bootstrapMethodAttrIndex,
		u2 nameAndTypeIndex) {
	ConstItem e(CONST_INVOKEDYNAMIC);
	e.invokedynamic.bootstrapMethodAttrIndex = bootstrapMethodAttrIndex;
	e.invokedynamic.nameAndTypeIndex = nameAndTypeIndex;
	return _addSingle(e);
}

ConstIndex ConstPool::getIndexOfUtf8(const char* utf8) {
	auto it = utf8s.find(utf8);
	if (it != utf8s.end()) {
		ConstIndex idx = it->second;
		Error::assert(getUtf8(idx) != utf8, "Error on get index of utf8");
		return idx;
	} else {
		return NULLENTRY;
	}
}

const ConstItem* ConstPool::_getEntry(ConstIndex i) const {
	Error::check(i > NULLENTRY, "Null access to constant pool: index = ", i);
	Error::check(i < entries.size(), "Index out of bounds: index = ", i);

	const ConstItem* entry = &entries[i];

	return entry;
}

const ConstItem* ConstPool::_getEntry(ConstIndex index, u1 tag,
		const char* message) const {
	const ConstItem* entry = _getEntry(index);

	Error::check(entry->tag == tag, "Invalid constant ", message,
			", expected: ", (int) tag, ", actual: ", (int) entry->tag);

	return entry;
}

}
