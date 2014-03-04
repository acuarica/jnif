#ifndef JNIF_CONSTPOOL_HPP
#define JNIF_CONSTPOOL_HPP

#include <vector>
#include <string>

#include "../base.hpp"

namespace jnif {

/**
 * Constant pool enum
 */
#define CONSTANT_Class				7
#define CONSTANT_Fieldref			9
#define CONSTANT_Methodref			10
#define CONSTANT_InterfaceMethodref	11
#define CONSTANT_String				8
#define CONSTANT_Integer			3
#define CONSTANT_Float				4
#define CONSTANT_Long				5
#define CONSTANT_Double				6
#define CONSTANT_NameAndType		12
#define CONSTANT_Utf8				1
#define CONSTANT_MethodHandle		15
#define CONSTANT_MethodType			16
#define CONSTANT_InvokeDynamic		18

class ConstPool {
public:

	struct Entry {
		u1 tag;
		union {
			struct {
				u2 name_index;
			} clazz;
			struct {
				u2 class_index;
				u2 name_and_type_index;
			} memberref;
			struct {
				u2 string_index;
			} s;
			struct {
				u4 value;
			} i;
			struct {
				u4 value;
			} f;
			struct {
				u4 high_bytes;
				u4 low_bytes;
			} l;
			struct {
				u4 high_bytes;
				u4 low_bytes;
			} d;
			struct {
				u2 name_index;
				u2 descriptor_index;
			} nameandtype;
			struct {
				u1 reference_kind;
				u2 reference_index;
			} methodhandle;
			struct {
				u2 descriptor_index;
			} methodtype;
			struct {
				u2 bootstrap_method_attr_index;
				u2 name_and_type_index;
			} invokedynamic;
		};
		struct {
			std::string str;
		} utf8;
	};

	ConstPool() {
		Entry nullEntry;
		entries.push_back(nullEntry);
	}

	u2 addSingle(const Entry& entry) {
		u2 index = entries.size();

		entries.push_back(entry);

		return index;
	}

	void addDouble(const Entry& entry) {
		entries.push_back(entry);

		Entry nullEntry;
		entries.push_back(nullEntry);
	}

	const std::string& getUtf8(int utf8Index) const {
		const Entry* entry = getEntry(utf8Index, CONSTANT_Utf8, "Utf8");

		return entry->utf8.str;
	}

	const std::string& getClazzName(int classIndex) const {
		u2 classNameIndex = getClazzNameIndex(classIndex);

		return getUtf8(classNameIndex);
	}

	void getNameAndType(int index, std::string* name, std::string* desc) const {
		const Entry* entry = getEntry(index, CONSTANT_NameAndType,
				"NameAndType");

		u2 nameIndex = entry->nameandtype.name_index;
		u2 descIndex = entry->nameandtype.descriptor_index;

		*name = getUtf8(nameIndex);
		*desc = getUtf8(descIndex);
	}

	void getMemberRef(int index, std::string* clazzName, std::string* name, std::string* desc,
			u1 tag) const {
		const Entry* entry = getEntry(index, tag, "memberref");

		u2 classIndex = entry->memberref.class_index;
		u2 nameAndTypeIndex = entry->memberref.name_and_type_index;

		*clazzName = getClazzName(classIndex);
		getNameAndType(nameAndTypeIndex, name, desc);
	}

	u2 addInteger(u4 value) {
		Entry e;
		e.tag = CONSTANT_Integer;
		e.i.value = value;

		return addSingle(e);
	}

	u2 addUtf8(const char* str) {
		Entry e;
		e.tag = CONSTANT_Utf8;
		e.utf8.str = str;

		return addSingle(e);
	}

	u2 addClass(u2 classNameIndex) {
		Entry e;
		e.tag = CONSTANT_Class;
		e.clazz.name_index = classNameIndex;

		return addSingle(e);
	}

	u2 addClass(const char* className) {
		u2 classNameIndex = addUtf8(className);
		return addClass(classNameIndex);
	}

	u2 addNameAndType(u2 nameIndex, u2 descIndex) {
		Entry e;
		e.tag = CONSTANT_NameAndType;
		e.nameandtype.name_index = nameIndex;
		e.nameandtype.descriptor_index = descIndex;

		return addSingle(e);
	}

	u2 addMethodRef(u2 classIndex, u2 nameAndTypeIndex) {
		Entry e;
		e.tag = CONSTANT_Methodref;
		e.memberref.class_index = classIndex;
		e.memberref.name_and_type_index = nameAndTypeIndex;

		return addSingle(e);
	}

	u2 addMethodRef(u2 classIndex, const char* name, const char* desc) {
		u2 methodNameIndex = addUtf8(name);
		u2 methodDescIndex = addUtf8(desc);
		u2 nameAndTypeIndex = addNameAndType(methodNameIndex, methodDescIndex);
		u2 methodRefIndex = addMethodRef(classIndex, nameAndTypeIndex);

		return methodRefIndex;
	}

private:
	const Entry* getEntry(u4 index, u1 tag, const char* message) const {
		const u2 NULLENTRY = 0;

		ASSERT(index > NULLENTRY, "Null access to constant pool %s index: %d",
				message, index);
		ASSERT(index < entries.size(), "Index for %s out of bounds %d", message,
				index);

		const Entry* entry = &entries[index];

		ASSERT(entry->tag == tag, "Invalid constant %s tag", message);

		return entry;
	}

	u2 getClazzNameIndex(int classIndex) const {
		const u2 NULLENTRY = 0;

		const Entry* entry = getEntry(classIndex, CONSTANT_Class,
				"CONSTANT_Class");

		u2 classNameIndex = entry->clazz.name_index;

		ASSERT(classNameIndex != NULLENTRY, "invalid class name index: %d",
				classNameIndex);

		return classNameIndex;
	}

public:
	std::vector<Entry> entries;
};


}

#endif
