/*
 * ConstPool.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_CONSTPOOL_HPP
#define JNIF_CONSTPOOL_HPP

#include "base.hpp"
#include <vector>
#include <map>

namespace jnif {

/**
 * Constant pool enum used to distinguish between different kinds of elements
 * inside the constant pool.
 */
enum ConstTag {

	/**
	 * Represents the null entry which cannot be addressed. This is used for
	 * the NULLENTRY (position zero) and for long and double entries.
	 */
	CONST_NULLENTRY,

	/**
	 * Represents a class or an interface.
	 */
	CONST_CLASS = 7,

	/**
	 * Represents a field.
	 */
	CONST_FIELDREF = 9,

	/**
	 * Represents a method.
	 */
	CONST_METHODREF = 10,

	/**
	 * Represents an inteface method.
	 */
	CONST_INTERMETHODREF = 11,

	/**
	 * Used to represent constant objects of the type String.
	 */
	CONST_STRING = 8,

	/**
	 * Represents 4-byte numeric int constants.
	 */
	CONST_INTEGER = 3,

	/**
	 * Represents 4-byte numeric float constants.
	 */
	CONST_FLOAT = 4,

	/**
	 * Represents 8-byte numeric long constants.
	 */
	CONST_LONG = 5,

	/**
	 * Represents 8-byte numeric double constants.
	 */
	CONST_DOUBLE = 6,

	/**
	 * Used to represent a field or method, without indicating which class
	 * or interface type it belongs to.
	 */
	CONST_NAMEANDTYPE = 12,

	/**
	 * Used to represent constant string values.
	 */
	CONST_UTF8 = 1,

	/**
	 * Used to represent a method handle.
	 */
	CONST_METHODHANDLE = 15,

	/**
	 * Used to represent a method type.
	 */
	CONST_METHODTYPE = 16,

	/**
	 * Used by an invokedynamic instruction to specify a bootstrap method,
	 * the dynamic invocation name, the argument and return types of the call,
	 * and optionally, a sequence of additional constants called static
	 * arguments to the bootstrap method.
	 */
	CONST_INVOKEDYNAMIC = 18
};

/**
 * The ConstIndex type represents how each item within the constant pool can
 * be addressed. The specification indicates that this is an u2 value.
 */
typedef u2 ConstIndex;

/**
 * Represents a class, interface or array type.
 *
 * @see CONST_CLASS
 */
class ConstClass {
public:

//	ConstClass(ConstIndex nameIndex) :
//			nameIndex(nameIndex) {
//	}

	/**
	 * The name of the class of this type.
	 *
	 * @see CONST_UTF8
	 * @see ConstUtf8
	 */
	ConstIndex nameIndex;
};

/**
 * Represents a symbolic reference to a member
 * (field, method or interface method) of a class.
 *
 * @see ConstFieldRef
 * @see ConstMethodRef
 * @see ConstInterMethodRef
 */
class ConstMemberRef {
public:

//	ConstMemberRef(ConstIndex classIndex, ConstIndex nameAndTypeIndex) :
//			classIndex(classIndex), nameAndTypeIndex(nameAndTypeIndex) {
//	}

	/**
	 * Index to the class of this member reference.
	 *
	 * @see ConstClass
	 */
	ConstIndex classIndex;

	/**
	 * Index to the name and type of this member reference.
	 *
	 * @see ConstNameAndType
	 */
	ConstIndex nameAndTypeIndex;
};

/**
 * Represents a string constant value.
 *
 * @see CONST_STRING
 */
class ConstString {
public:

	/**
	 * The index of the utf8 entry containing this string value.
	 *
	 * @see ConstUtf8
	 */
	ConstIndex stringIndex;
};

/**
 * Represents an integer constant value.
 *
 * @see CONST_INTEGER
 */
class ConstInteger {
public:

	/**
	 * The integer value of this entry.
	 */
	int value;
};

/**
 * Represent a float constant value.
 */
class ConstFloat {
public:

	/**
	 * The float value of this entry.
	 */
	float value;
};

/**
 * Represents a long constant value.
 *
 * @see CONST_LONG
 */
class ConstLong {
public:

	/**
	 * The long value of this entry.
	 */
	long value;
};

/**
 * Represents a double constant value.
 *
 * @see CONST_DOUBLE
 */
class ConstDouble {
public:

	/**
	 * The double value of this entry.
	 */
	double value;
};

/**
 * Represents a tuple of name and descriptor.
 */
class ConstNameAndType {
public:

	/**
	 * The index of the utf8 entry containing the name of this entry.
	 */
	ConstIndex nameIndex;

	/**
	 * The index of the utf8 entry containing the type
	 * descriptor of this entry.
	 */
	ConstIndex descriptorIndex;
};

/**
 * Contains a modified UTF-8 string.
 *
 * @see CONST_UTF8
 */
class ConstUtf8 {
public:

	ConstUtf8() {
	}

	ConstUtf8(const String& str) :
			str(str) {
	}

	/**
	 * The string data.
	 */
	String str;
};

/**
 * Represents a method handle entry.
 *
 * @see CONST_METHODHANDLE
 */
class ConstMethodHandle {
public:

	/**
	 * The reference kind of this entry.
	 */
	u1 referenceKind;

	/**
	 * The reference index of this entry.
	 */
	u2 referenceIndex;
};

/**
 * Represents the type of a method.
 *
 * @see CONST_METHODTYPE
 */
class ConstMethodType {
public:

	/**
	 * The utf8 index containing the descriptor of this entry.
	 */
	ConstIndex descriptorIndex;
};

/**
 * Represents an invoke dynamic call site.
 *
 * @see CONST_INVOKEDYNAMIC
 */
class ConstInvokeDynamic {
public:

	/**
	 * The bootstrap method attribute index.
	 */
	ConstIndex bootstrapMethodAttrIndex;

	/**
	 * The name and type index of this entry.
	 *
	 * @see ConstNameAndType
	 */
	ConstIndex nameAndTypeIndex;
};

/**
 * The const item.
 */
class ConstItem {

	friend class ConstPool;

public:

	ConstTag tag;

	union {
		ConstClass clazz;
		ConstMemberRef memberRef;
		ConstString s;
		ConstInteger i;
		ConstFloat f;
		ConstLong l;
		ConstDouble d;
		ConstNameAndType nameandtype;
		ConstMethodHandle methodhandle;
		ConstMethodType methodtype;
		ConstInvokeDynamic invokedynamic;
	};

	ConstUtf8 utf8;

private:

	ConstItem(ConstTag tag) :
			tag(tag) {
	}

	ConstItem(ConstTag tag, ConstIndex classIndex, ConstIndex nameAndTypeIndex) :
			tag(tag) {
		memberRef.classIndex = classIndex;
		memberRef.nameAndTypeIndex = nameAndTypeIndex;
	}

	ConstItem(ConstTag tag, int value) :
			tag(tag) {
		i.value = value;
	}

	ConstItem(ConstTag tag, float value) :
			tag(tag) {
		f.value = value;
	}

	ConstItem(ConstTag tag, long value) :
			tag(tag) {
		l.value = value;
	}

	ConstItem(ConstTag tag, double value) :
			tag(tag) {
		d.value = value;
	}

	ConstItem(ConstTag tag, const String& value) :
			tag(tag), utf8(value) {
	}

};

/**
 * Represents the Java class file's constant pool. Provides the base services
 * to manage the constant pool. The constant pool is a table which holds
 * different kinds of items depending on their use. It can
 * hold references for classes, fields, methods, interface methods, strings,
 * integers, floats, longs, doubles, name and type, utf8 arrays,
 * method handles, method types and invoke dynamic bootstrap methods.
 *
 * This class works by adding these kinds of entries in an array-like
 * structure. When an entry is removed, all entries after the one removed will
 * be invalidated, for this reason, no removed operations are allowed.
 */
class ConstPool {
	friend class ClassFile;

public:

	/**
	 * Defines how to iterate the constant pool.
	 */
	class Iterator {
		friend class ConstPool;
	public:

		bool hasNext() const {
			return index < cp.size();
		}

		ConstIndex operator*() const {
			return index;
		}

		Iterator& operator++(int) {
			index += cp._isDoubleEntry(index) ? 2 : 1;

			return *this;
		}

	private:
		Iterator(const ConstPool& cp, ConstIndex index) :
				cp(cp), index(index) {
		}

		const ConstPool& cp;
		ConstIndex index;
	};

	/**
	 * Represents the invalid (null) item, which must not be asked for.
	 */
	static const ConstIndex NULLENTRY = 0;

	/**
	 * Returns the number of elements in this constant pool.
	 *
	 * @returns number of elements in this constant pool.
	 */
	u4 size() const;

	/**
	 *
	 */
	Iterator iterator() const;

	/**
	 * Adds a class reference to the constant pool.
	 *
	 * @param classNameIndex the utf8 index that represents the name of this
	 * class item.
	 * @returns the index of the newly created reference to a class item.
	 */
	ConstIndex addClass(ConstIndex classNameIndex);

	/**
	 * Adds a class reference to the constant pool by class name. This method
	 * adds also the utf8 entry corresponding the class name itself.
	 *
	 * @param className name of the class to reference.
	 * @returns the index of the newly created reference to a class item.
	 */
	ConstIndex addClass(const char* className);

	/**
	 * Adds a field reference to this constant pool.
	 *
	 * @param classIndex the symbolic class that this field belongs to.
	 * @param nameAndTypeIndex the name and type symbolic reference
	 * describing the name and type of the field to add.
	 * @returns the index of the newly created entry.
	 */
	ConstIndex addFieldRef(ConstIndex classIndex, ConstIndex nameAndTypeIndex);

	/**
	 * Adds a class method reference.
	 *
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addMethodRef(ConstIndex classIndex, ConstIndex nameAndTypeIndex);

	/**
	 * Adds a non-interface methods by method name and descriptor.
	 *
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addMethodRef(ConstIndex classIndex, const char* name,
			const char* desc);

	/**
	 * Adds an interface method reference.
	 *
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addInterMethodRef(ConstIndex classIndex,
			ConstIndex nameAndTypeIndex);

	/**
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addString(ConstIndex utf8Index);

	/**
	 * Adds a string to the constant pool by providing a string.
	 */
	ConstIndex addString(const String& str);

	/**
	 * Adds a String item to the Constant Pool using a class entry.
	 *
	 * @returns the ConstIndex of the newly created String entry.
	 */
	ConstIndex addStringFromClass(ConstIndex classIndex);

	/**
	 * Adds an integer constant value.
	 *
	 * @param value the integer value.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addInteger(int value);

	/**
	 * Adds a float constant value.
	 *
	 * @param value the float value.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addFloat(float value);

	/**
	 * Adds a long constant value.
	 *
	 * @param value the long value.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addLong(long value);

	/**
	 * Adds a double constant value.
	 *
	 * @param value the double value.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addDouble(double value);

	/**
	 * Adds a name and type descriptor pair.
	 *
	 * @param nameIndex the index of the UTF8 entry with the name.
	 * @param descIndex the index of the UTF8 entry with the type descriptor.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addNameAndType(ConstIndex nameIndex, ConstIndex descIndex);

	/**
	 * Adds a modified UTF8 string given the char array and its len.
	 *
	 * @param utf8 the char array containing the modified UTF8 string.
	 * @param len the len in bytes of utf8.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addUtf8(const char* utf8, int len);

	/**
	 * Adds a modified UTF8 string given an null-terminated char array.
	 *
	 * @param str the null-terminated char array containing the modified
	 * UTF8 string.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addUtf8(const char* str);

	/**
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addMethodHandle(u1 refKind, u2 refIndex);

	/**
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addMethodType(u2 descIndex);

	/**
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addInvokeDynamic(u2 bootstrapMethodAttrIndex,
			u2 nameAndTypeIndex);

	/**
	 *
	 */
	ConstTag getTag(ConstIndex index) const {
		const ConstItem* entry = _getEntry(index);
		return entry->tag;
	}

	/**
	 * Checks whether the requested index holds a class reference.
	 */
	bool isClass(ConstIndex index) const {
		return _getEntry(index)->tag == CONST_CLASS;
	}

	bool isUtf8(ConstIndex index) const {
		return _getEntry(index)->tag == CONST_UTF8;
	}

	ConstIndex getClassNameIndex(ConstIndex classIndex) const {
		const ConstItem* e = _getEntry(classIndex, CONST_CLASS,
				"CONSTANT_Class");

		ConstIndex ni = e->clazz.nameIndex;

		return ni;
	}

	void getFieldRef(ConstIndex index, String* className, String* name,
			String* desc) const {
		const ConstItem* e = _getEntry(index, CONST_FIELDREF, "FieldRef");
		const ConstMemberRef& mr = e->memberRef;
		_getMemberRef(className, name, desc, mr);
	}

	void getMethodRef(ConstIndex index, String* clazzName, String* name,
			String* desc) const {
		const ConstItem* e = _getEntry(index, CONST_METHODREF, "MethodRef");
		const ConstMemberRef& mr = e->memberRef;
		_getMemberRef(clazzName, name, desc, mr);
	}

	void getInterMethodRef(ConstIndex index, String* clazzName, String* name,
			String* desc) const {
		const ConstItem* e = _getEntry(index, CONST_INTERMETHODREF, "imr");
		const ConstMemberRef& mr = e->memberRef;
		_getMemberRef(clazzName, name, desc, mr);
	}

	long getLong(ConstIndex index) const {
		return _getEntry(index, CONST_LONG, "CONSTANT_Long")->l.value;
	}

	double getDouble(ConstIndex index) const {
		return _getEntry(index, CONST_DOUBLE, "CONSTANT_Double")->d.value;
	}

	const char* getUtf8(ConstIndex utf8Index) const {
		const ConstItem* entry = _getEntry(utf8Index, CONST_UTF8, "Utf8");

		return entry->utf8.str.c_str();
	}

	const char* getClassName(ConstIndex classIndex) const {
		ConstIndex classNameIndex = getClassNameIndex(classIndex);

		return getUtf8(classNameIndex);
	}

	void getNameAndType(ConstIndex index, String* name, String* desc) const {
		const ConstItem* e = _getEntry(index, CONST_NAMEANDTYPE, "NameAndType");

		u2 nameIndex = e->nameandtype.nameIndex;
		u2 descIndex = e->nameandtype.descriptorIndex;

		*name = getUtf8(nameIndex);
		*desc = getUtf8(descIndex);
	}

	const ConstInvokeDynamic& getInvokeDynamic(ConstIndex index) const {
		const ConstItem* e = _getEntry(index, CONST_INVOKEDYNAMIC, "Indy");
		return e->invokedynamic;
	}

	ConstIndex getIndexOfUtf8(const char* utf8);

	ConstIndex putUtf8(const char* utf8) {
		ConstIndex i = getIndexOfUtf8(utf8);
		if (i == NULLENTRY) {
			return addUtf8(utf8);
		} else {
			return i;
		}
	}

	std::vector<ConstItem> entries;

private:

	/**
	 * Initializes an empty constant pool. The valid indices start from 1
	 * inclusive, because the null entry (index 0) is added by default.
	 */
	ConstPool() {
		entries.reserve(4096);

		ConstItem nullEntry(CONST_NULLENTRY);
		entries.push_back(nullEntry);
	}

	ConstIndex _addSingle(const ConstItem& entry) {
		ConstIndex index = entries.size();
		entries.push_back(entry);

		return index;
	}

	ConstIndex _addDoubleEntry(const ConstItem& entry) {
		ConstIndex index = entries.size();
		entries.push_back(entry);

		ConstItem nullEntry(CONST_NULLENTRY);
		entries.push_back(nullEntry);

		return index;
	}

	const ConstItem* _getEntry(ConstIndex i) const;

	const ConstItem* _getEntry(ConstIndex index, u1 tag,
			const char* message) const;

	bool _isDoubleEntry(ConstIndex index) const {
		const ConstItem* e = _getEntry(index);
		return e->tag == CONST_LONG || e->tag == CONST_DOUBLE;
	}

	void _getMemberRef(String* clazzName, String* name, String* desc,
			const ConstMemberRef& memberRef) const {
		ConstIndex classIndex = memberRef.classIndex;
		ConstIndex nameAndTypeIndex = memberRef.nameAndTypeIndex;

		*clazzName = getClassName(classIndex);
		getNameAndType(nameAndTypeIndex, name, desc);
	}

	std::map<String, ConstIndex> utf8s;
};

std::ostream& operator<<(std::ostream& os, const ConstTag& tag);

}

#endif
