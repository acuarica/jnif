#include "jnif.hpp"

namespace jnif {

ConstPool::ConstPool() {
	ConstPoolEntry nullEntry;
	entries.push_back(nullEntry);
}

u2 ConstPool::addSingle(const ConstPoolEntry& entry) {
	u2 index = entries.size();

	entries.push_back(entry);

	return index;
}

void ConstPool::addDouble(const ConstPoolEntry& entry) {
	entries.push_back(entry);

	ConstPoolEntry nullEntry;
	entries.push_back(nullEntry);
}

const string& ConstPool::getUtf8(int utf8Index) const {
	const ConstPoolEntry* entry = getEntry(utf8Index, CONSTANT_Utf8, "Utf8");

	return entry->utf8.str;
}

const string& ConstPool::getClazzName(int classIndex) const {
	u2 classNameIndex = getClazzNameIndex(classIndex);

	return getUtf8(classNameIndex);
}

void ConstPool::getNameAndType(int index, string* name, string* desc) const {
	const ConstPoolEntry* entry = getEntry(index, CONSTANT_NameAndType,
			"NameAndType");

	u2 nameIndex = entry->nameandtype.name_index;
	u2 descIndex = entry->nameandtype.descriptor_index;

	*name = getUtf8(nameIndex);
	*desc = getUtf8(descIndex);
}

void ConstPool::getMemberRef(int index, std::string* clazzName,
		std::string* name, std::string* desc, u1 tag) const {
	const ConstPoolEntry* entry = getEntry(index, tag, "memberref");

	u2 classIndex = entry->memberref.class_index;
	u2 nameAndTypeIndex = entry->memberref.name_and_type_index;

	*clazzName = getClazzName(classIndex);
	getNameAndType(nameAndTypeIndex, name, desc);
}

u2 ConstPool::addInteger(u4 value) {
	ConstPoolEntry e;
	e.tag = CONSTANT_Integer;
	e.i.value = value;

	return addSingle(e);
}

u2 ConstPool::addUtf8(const char* str) {
	ConstPoolEntry e;
	e.tag = CONSTANT_Utf8;
	e.utf8.str = str;

	return addSingle(e);
}

u2 ConstPool::addClass(u2 classNameIndex) {
	ConstPoolEntry e;
	e.tag = CONSTANT_Class;
	e.clazz.name_index = classNameIndex;

	return addSingle(e);
}

u2 ConstPool::addClass(const char* className) {
	u2 classNameIndex = addUtf8(className);
	return addClass(classNameIndex);
}

u2 ConstPool::addNameAndType(u2 nameIndex, u2 descIndex) {
	ConstPoolEntry e;
	e.tag = CONSTANT_NameAndType;
	e.nameandtype.name_index = nameIndex;
	e.nameandtype.descriptor_index = descIndex;

	return addSingle(e);
}

u2 ConstPool::addMethodRef(u2 classIndex, u2 nameAndTypeIndex) {
	ConstPoolEntry e;
	e.tag = CONSTANT_Methodref;
	e.memberref.class_index = classIndex;
	e.memberref.name_and_type_index = nameAndTypeIndex;

	return addSingle(e);
}

u2 ConstPool::addMethodRef(u2 classIndex, const char* name, const char* desc) {
	u2 methodNameIndex = addUtf8(name);
	u2 methodDescIndex = addUtf8(desc);
	u2 nameAndTypeIndex = addNameAndType(methodNameIndex, methodDescIndex);
	u2 methodRefIndex = addMethodRef(classIndex, nameAndTypeIndex);

	return methodRefIndex;
}

const ConstPoolEntry* ConstPool::getEntry(u4 index, u1 tag,
		const char* message) const {
	const u2 NULLENTRY = 0;

	ASSERT(index > NULLENTRY, "Null access to constant pool %s index: %d",
			message, index);
	ASSERT(index < entries.size(), "Index for %s out of bounds %d", message,
			index);

	const ConstPoolEntry* entry = &entries[index];

	ASSERT(entry->tag == tag, "Invalid constant %s tag", message);

	return entry;
}

u2 ConstPool::getClazzNameIndex(int classIndex) const {
	const u2 NULLENTRY = 0;

	const ConstPoolEntry* entry = getEntry(classIndex, CONSTANT_Class,
			"CONSTANT_Class");

	u2 classNameIndex = entry->clazz.name_index;

	ASSERT(classNameIndex != NULLENTRY, "invalid class name index: %d",
			classNameIndex);

	return classNameIndex;
}

}

