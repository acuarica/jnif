#ifndef JNIF_HPP
#define JNIF_HPP

#include <string>
#include <vector>
#include <list>
#include <ostream>

/**
 * The jnif namespace contains all type definitions, constants, enumerations
 * and structs of the jnif framework.
 *
 * This implementation is based on Chapter 4 (The class File Format) and
 * Chapter 6 (Chapter 6. The Java Virtual Machine Instruction Set) of the
 * Java Virtual Machine Specification version 7. Portions of this
 * documentation are taken from this specification.
 *
 * For more information refer to:
 *
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
 *
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-6.html
 *
 * @see ClassFile
 *
 */
namespace jnif {

/**
 * Represents a byte inside the Java Class File.
 * The sizeof(u1) must be equal to 1.
 */
typedef unsigned char u1;

/**
 * Represents two bytes inside the Java Class File.
 * The sizeof(u2) must be equal to 2.
 */
typedef unsigned short u2;

/**
 * Represents four bytes inside the Java Class File.
 * The sizeof(u4) must be equal to 4.
 */
typedef unsigned int u4;

/**
 * We use STL string.
 */
typedef std::string String;

/**
 * Represents the exception that jnif can throw.
 */
class JnifException {
public:

	/**
	 * Creates an exception given the message and the stack trace.
	 *
	 * @param message contains information about exceptional situation.
	 * @param stackTrace the stack trace where this exception happened.
	 */
	JnifException(const String& message, const String& stackTrace) :
			message(message), stackTrace(stackTrace) {
	}

	/**
	 * Contains information about the exceptional situation.
	 */
	String message;

	/**
	 * the stack trace where this exception happened.
	 */
	String stackTrace;

};

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
	 * The index of the utf8 entry containing the type descriptor of this entry.
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
	 * The bootstrap method attribute idnex.
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

	const ConstTag tag;

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
			tag(tag), memberRef( { classIndex, nameAndTypeIndex }) {
	}

	ConstItem(ConstTag tag, int value) :
			tag(tag), i( { value }) {
	}

	ConstItem(ConstTag tag, float value) :
			tag(tag), f( { value }) {
	}

	ConstItem(ConstTag tag, long value) :
			tag(tag), l( { value }) {
	}

	ConstItem(ConstTag tag, double value) :
			tag(tag), d( { value }) {
	}

	ConstItem(ConstTag tag, const String& value) :
			tag(tag), utf8( { value }) {
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
	u4 size() const {
		return entries.size();
	}

	/**
	 *
	 */
	Iterator iterator() const {
		return Iterator(*this, 1);
	}

	/**
	 * Adds a class reference to the constant pool.
	 *
	 * @param classNameIndex the utf8 index that represents the name of this
	 * class item.
	 * @returns the index of the newly created reference to a class item.
	 */
	ConstIndex addClass(ConstIndex classNameIndex) {
		ConstItem e(CONST_CLASS);
		e.clazz.nameIndex = classNameIndex;

		return _addSingle(e);
	}

	/**
	 * Adds a class reference to the constant pool by class name. This method
	 * adds also the utf8 entry corresponding the class name itself.
	 *
	 * @param className name of the class to reference.
	 * @returns the index of the newly created reference to a class item.
	 */
	ConstIndex addClass(const char* className) {
		ConstIndex classNameIndex = addUtf8(className);
		return addClass(classNameIndex);
	}

	/**
	 * Adds a field reference to this constant pool.
	 *
	 * @param classIndex the symbolic class that this field belongs to.
	 * @param nameAndTypeIndex the name and type symbolic reference
	 * describing the name and type of the field to add.
	 * @returns the index of the newly created entry.
	 */
	ConstIndex addFieldRef(ConstIndex classIndex, ConstIndex nameAndTypeIndex) {
		ConstItem e(CONST_FIELDREF, classIndex, nameAndTypeIndex);
		return _addSingle(e);
	}

	/**
	 * Adds a class method reference.
	 *
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addMethodRef(ConstIndex classIndex,
			ConstIndex nameAndTypeIndex) {
		ConstItem e(CONST_METHODREF, classIndex, nameAndTypeIndex);
		return _addSingle(e);
	}

	/**
	 * Adds a non-interface methods by method name and descriptor.
	 *
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addMethodRef(ConstIndex classIndex, const char* name,
			const char* desc) {
		ConstIndex methodNameIndex = addUtf8(name);
		ConstIndex methodDescIndex = addUtf8(desc);
		ConstIndex nameAndTypeIndex = addNameAndType(methodNameIndex,
				methodDescIndex);
		ConstIndex methodRefIndex = addMethodRef(classIndex, nameAndTypeIndex);

		return methodRefIndex;
	}

	/**
	 * Adds an interface method reference.
	 *
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addInterMethodRef(ConstIndex classIndex,
			ConstIndex nameAndTypeIndex) {
		ConstItem e(CONST_INTERMETHODREF, classIndex, nameAndTypeIndex);
		return _addSingle(e);
	}

	/**
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addString(ConstIndex utf8Index) {
		ConstItem e(CONST_STRING);
		e.s.stringIndex = utf8Index;

		return _addSingle(e);
	}

	/**
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addStringFromClass(ConstIndex classIndex) {
		ConstIndex classNameIndex = getClassNameIndex(classIndex);
		ConstIndex classNameStringIndex = addString(classNameIndex);

		return classNameStringIndex;
	}

	/**
	 * Adds an integer constant value.
	 *
	 * @param value the integer value.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addInteger(int value) {
		ConstItem e(CONST_INTEGER, value);
		return _addSingle(e);
	}

	/**
	 * Adds a float constant value.
	 *
	 * @param value the float value.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addFloat(float value) {
		ConstItem e(CONST_FLOAT, value);
		return _addSingle(e);
	}

	/**
	 * Adds a long constant value.
	 *
	 * @param value the long value.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addLong(long value) {
		ConstItem e(CONST_LONG, value);
		return _addDoubleEntry(e);
	}

	/**
	 * Adds a double constant value.
	 *
	 * @param value the double value.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addDouble(double value) {
		ConstItem entry(CONST_DOUBLE, value);
		return _addDoubleEntry(entry);
	}

	/**
	 * Adds a name and type descriptor pair.
	 *
	 * @param nameIndex the index of the UTF8 entry with the name.
	 * @param descIndex the index of the UTF8 entry with the type descriptor.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addNameAndType(ConstIndex nameIndex, ConstIndex descIndex) {
		ConstItem e(CONST_NAMEANDTYPE);
		e.nameandtype.nameIndex = nameIndex;
		e.nameandtype.descriptorIndex = descIndex;

		return _addSingle(e);
	}

	/**
	 * Adds a modified UTF8 string given the char array and its len.
	 *
	 * @param utf8 the char array containing the modified UTF8 string.
	 * @param len the len in bytes of utf8.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addUtf8(const char* utf8, int len) {
		String str(utf8, len);
		ConstItem e(CONST_UTF8, str);
		return _addSingle(e);
	}

	/**
	 * Adds a modified UTF8 string given an null-terminated char array.
	 *
	 * @param str the null-terminated char array containing the modified
	 * UTF8 string.
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addUtf8(const char* str) {
		ConstItem e(CONST_UTF8, str);
		return _addSingle(e);
	}

	/**
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addMethodHandle(u1 refKind, u2 refIndex) {
		ConstItem e(CONST_METHODHANDLE);
		e.methodhandle.referenceKind = refKind;
		e.methodhandle.referenceIndex = refIndex;
		return _addSingle(e);
	}

	/**
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addMethodType(u2 descIndex) {
		ConstItem e(CONST_METHODTYPE);
		e.methodtype.descriptorIndex = descIndex;
		return _addSingle(e);
	}

	/**
	 * @returns the ConstIndex of the newly created entry.
	 */
	ConstIndex addInvokeDynamic(u2 bootstrapMethodAttrIndex,
			u2 nameAndTypeIndex) {
		ConstItem e(CONST_INVOKEDYNAMIC);
		e.invokedynamic.bootstrapMethodAttrIndex = bootstrapMethodAttrIndex;
		e.invokedynamic.nameAndTypeIndex = nameAndTypeIndex;
		return _addSingle(e);
	}

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

	ConstIndex getIndexOfUtf8(const char* utf8) {

		ConstPool& cp = *this;
		for (ConstPool::Iterator it = cp.iterator(); it.hasNext(); it++) {
			ConstIndex i = *it;
			//ConstPool::Tag tag = cp.getTag(i);

			//const Entry* entry = &cp.entries[i];

			if (isUtf8(i) && getUtf8(i) == String(utf8)) {
				return i;
			}
		}

		return NULLENTRY;
	}

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
}
;

/**
 * Access flags for the class itself.
 */
enum ClassFlags {

	/**
	 * Declared public; may be accessed from outside its package.
	 */
	CLASS_PUBLIC = 0x0001,

	/**
	 * Declared final; no subclasses allowed.
	 */
	CLASS_FINAL = 0x0010,

	/**
	 * Treat superclass methods specially when invoked by the
	 * invokespecial instruction.
	 */
	CLASS_SUPER = 0x0020,

	/**
	 * Is an interface, not a class.
	 */
	CLASS_INTERFACE = 0x0200,

	/**
	 * Declared abstract; must not be instantiated.
	 */
	CLASS_ABSTRACT = 0x0400,

	/**
	 * Declared synthetic; not present in the source code.
	 */
	CLASS_SYNTHETIC = 0x1000,

	/**
	 * Declared as an annotation type.
	 */
	CLASS_ANNOTATION = 0x2000,

	/**
	 * Declared as an enum type.
	 */
	CLASS_ENUM = 0x4000
};

/**
 * Access flags used by methods.
 */
enum MethodFlags {

	/**
	 * Declared public; may be accessed from outside its package.
	 */
	METHOD_PUBLIC = 0x0001,

	/**
	 * Declared private; accessible only within the defining class.
	 */
	METHOD_PRIVATE = 0x0002,

	/**
	 * Declared protected; may be accessed within subclasses.
	 */
	METHOD_PROTECTED = 0x0004,

	/**
	 * Declared static.
	 */
	METHOD_STATIC = 0x0008,

	/**
	 * Declared final; must not be overridden (see 5.4.5).
	 */
	METHOD_FINAL = 0x0010,

	/**
	 * Declared synchronized; invocation is wrapped by a monitor use.
	 */
	METHOD_SYNCHRONIZED = 0x0020,

	/**
	 * A bridge method, generated by the compiler.
	 */
	METHOD_BRIDGE = 0x0040,

	/**
	 * Declared with variable number of arguments.
	 */
	METHOD_VARARGS = 0x0080,

	/**
	 * Declared native; implemented in a language other than Java.
	 */
	METHOD_NATIVE = 0x0100,

	/**
	 * Declared abstract; no implementation is provided.
	 */
	METHOD_ABSTRACT = 0x0400,

	/**
	 * Declared strictfp; floating-point mode is FP-strict.
	 */
	METHOD_STRICT = 0x0800,

	/**
	 * Declared synthetic; not present in the source code.
	 */
	METHOD_SYNTHETIC = 0x1000,
};

/**
 * Access flags used by fields.
 */
enum FieldFlags {

	/**
	 * Declared public; may be accessed from outside its package.
	 */
	FIELD_PUBLIC = 0x0001,

	/**
	 * Declared private; usable only within the defining class.
	 */
	FIELD_PRIVATE = 0x0002,

	/**
	 * Declared protected; may be accessed within subclasses.
	 */
	FIELD_PROTECTED = 0x0004,

	/**
	 * Declared static.
	 */
	FIELD_STATIC = 0x0008,
	/**
	 * Declared final;
	 * never directly assigned to after object construction (JLS $17.5).
	 */
	FIELD_FINAL = 0x0010,

	/**
	 * Declared volatile; cannot be cached.
	 */
	FIELD_VOLATILE = 0x0040,

	/**
	 * Declared transient; not written or read by a persistent object manager.
	 */
	FIELD_TRANSIENT = 0x0080,

	/**
	 * Declared synthetic; not present in the source code.
	 */
	FIELD_SYNTHETIC = 0x1000,

	/**
	 * Declared as an element of an enum.
	 */
	FIELD_ENUM = 0x4000
};

/**
 * OPCODES constants definitions.
 *
 * This enumeration type was taken from
 *
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-6.html
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-7.html
 */
enum Opcode {
	OPCODE_nop = 0x00,
	OPCODE_aconst_null = 0x01,
	OPCODE_iconst_m1 = 0x02,
	OPCODE_iconst_0 = 0x03,
	OPCODE_iconst_1 = 0x04,
	OPCODE_iconst_2 = 0x05,
	OPCODE_iconst_3 = 0x06,
	OPCODE_iconst_4 = 0x07,
	OPCODE_iconst_5 = 0x08,
	OPCODE_lconst_0 = 0x09,
	OPCODE_lconst_1 = 0x0a,
	OPCODE_fconst_0 = 0x0b,
	OPCODE_fconst_1 = 0x0c,
	OPCODE_fconst_2 = 0x0d,
	OPCODE_dconst_0 = 0x0e,
	OPCODE_dconst_1 = 0x0f,
	OPCODE_bipush = 0x10,
	OPCODE_sipush = 0x11,
	OPCODE_ldc = 0x12,
	OPCODE_ldc_w = 0x13,
	OPCODE_ldc2_w = 0x14,
	OPCODE_iload = 0x15,
	OPCODE_lload = 0x16,
	OPCODE_fload = 0x17,
	OPCODE_dload = 0x18,
	OPCODE_aload = 0x19,
	OPCODE_iload_0 = 0x1a,
	OPCODE_iload_1 = 0x1b,
	OPCODE_iload_2 = 0x1c,
	OPCODE_iload_3 = 0x1d,
	OPCODE_lload_0 = 0x1e,
	OPCODE_lload_1 = 0x1f,
	OPCODE_lload_2 = 0x20,
	OPCODE_lload_3 = 0x21,
	OPCODE_fload_0 = 0x22,
	OPCODE_fload_1 = 0x23,
	OPCODE_fload_2 = 0x24,
	OPCODE_fload_3 = 0x25,
	OPCODE_dload_0 = 0x26,
	OPCODE_dload_1 = 0x27,
	OPCODE_dload_2 = 0x28,
	OPCODE_dload_3 = 0x29,
	OPCODE_aload_0 = 0x2a,
	OPCODE_aload_1 = 0x2b,
	OPCODE_aload_2 = 0x2c,
	OPCODE_aload_3 = 0x2d,
	OPCODE_iaload = 0x2e,
	OPCODE_laload = 0x2f,
	OPCODE_faload = 0x30,
	OPCODE_daload = 0x31,
	OPCODE_aaload = 0x32,
	OPCODE_baload = 0x33,
	OPCODE_caload = 0x34,
	OPCODE_saload = 0x35,
	OPCODE_istore = 0x36,
	OPCODE_lstore = 0x37,
	OPCODE_fstore = 0x38,
	OPCODE_dstore = 0x39,
	OPCODE_astore = 0x3a,
	OPCODE_istore_0 = 0x3b,
	OPCODE_istore_1 = 0x3c,
	OPCODE_istore_2 = 0x3d,
	OPCODE_istore_3 = 0x3e,
	OPCODE_lstore_0 = 0x3f,
	OPCODE_lstore_1 = 0x40,
	OPCODE_lstore_2 = 0x41,
	OPCODE_lstore_3 = 0x42,
	OPCODE_fstore_0 = 0x43,
	OPCODE_fstore_1 = 0x44,
	OPCODE_fstore_2 = 0x45,
	OPCODE_fstore_3 = 0x46,
	OPCODE_dstore_0 = 0x47,
	OPCODE_dstore_1 = 0x48,
	OPCODE_dstore_2 = 0x49,
	OPCODE_dstore_3 = 0x4a,
	OPCODE_astore_0 = 0x4b,
	OPCODE_astore_1 = 0x4c,
	OPCODE_astore_2 = 0x4d,
	OPCODE_astore_3 = 0x4e,
	OPCODE_iastore = 0x4f,
	OPCODE_lastore = 0x50,
	OPCODE_fastore = 0x51,
	OPCODE_dastore = 0x52,
	OPCODE_aastore = 0x53,
	OPCODE_bastore = 0x54,
	OPCODE_castore = 0x55,
	OPCODE_sastore = 0x56,
	OPCODE_pop = 0x57,
	OPCODE_pop2 = 0x58,
	OPCODE_dup = 0x59,
	OPCODE_dup_x1 = 0x5a,
	OPCODE_dup_x2 = 0x5b,
	OPCODE_dup2 = 0x5c,
	OPCODE_dup2_x1 = 0x5d,
	OPCODE_dup2_x2 = 0x5e,
	OPCODE_swap = 0x5f,
	OPCODE_iadd = 0x60,
	OPCODE_ladd = 0x61,
	OPCODE_fadd = 0x62,
	OPCODE_dadd = 0x63,
	OPCODE_isub = 0x64,
	OPCODE_lsub = 0x65,
	OPCODE_fsub = 0x66,
	OPCODE_dsub = 0x67,
	OPCODE_imul = 0x68,
	OPCODE_lmul = 0x69,
	OPCODE_fmul = 0x6a,
	OPCODE_dmul = 0x6b,
	OPCODE_idiv = 0x6c,
	OPCODE_ldiv = 0x6d,
	OPCODE_fdiv = 0x6e,
	OPCODE_ddiv = 0x6f,
	OPCODE_irem = 0x70,
	OPCODE_lrem = 0x71,
	OPCODE_frem = 0x72,
	OPCODE_drem = 0x73,
	OPCODE_ineg = 0x74,
	OPCODE_lneg = 0x75,
	OPCODE_fneg = 0x76,
	OPCODE_dneg = 0x77,
	OPCODE_ishl = 0x78,
	OPCODE_lshl = 0x79,
	OPCODE_ishr = 0x7a,
	OPCODE_lshr = 0x7b,
	OPCODE_iushr = 0x7c,
	OPCODE_lushr = 0x7d,
	OPCODE_iand = 0x7e,
	OPCODE_land = 0x7f,
	OPCODE_ior = 0x80,
	OPCODE_lor = 0x81,
	OPCODE_ixor = 0x82,
	OPCODE_lxor = 0x83,
	OPCODE_iinc = 0x84,
	OPCODE_i2l = 0x85,
	OPCODE_i2f = 0x86,
	OPCODE_i2d = 0x87,
	OPCODE_l2i = 0x88,
	OPCODE_l2f = 0x89,
	OPCODE_l2d = 0x8a,
	OPCODE_f2i = 0x8b,
	OPCODE_f2l = 0x8c,
	OPCODE_f2d = 0x8d,
	OPCODE_d2i = 0x8e,
	OPCODE_d2l = 0x8f,
	OPCODE_d2f = 0x90,
	OPCODE_i2b = 0x91,
	OPCODE_i2c = 0x92,
	OPCODE_i2s = 0x93,
	OPCODE_lcmp = 0x94,
	OPCODE_fcmpl = 0x95,
	OPCODE_fcmpg = 0x96,
	OPCODE_dcmpl = 0x97,
	OPCODE_dcmpg = 0x98,
	OPCODE_ifeq = 0x99,
	OPCODE_ifne = 0x9a,
	OPCODE_iflt = 0x9b,
	OPCODE_ifge = 0x9c,
	OPCODE_ifgt = 0x9d,
	OPCODE_ifle = 0x9e,
	OPCODE_if_icmpeq = 0x9f,
	OPCODE_if_icmpne = 0xa0,
	OPCODE_if_icmplt = 0xa1,
	OPCODE_if_icmpge = 0xa2,
	OPCODE_if_icmpgt = 0xa3,
	OPCODE_if_icmple = 0xa4,
	OPCODE_if_acmpeq = 0xa5,
	OPCODE_if_acmpne = 0xa6,
	OPCODE_goto = 0xa7,
	OPCODE_jsr = 0xa8,
	OPCODE_ret = 0xa9,
	OPCODE_tableswitch = 0xaa,
	OPCODE_lookupswitch = 0xab,
	OPCODE_ireturn = 0xac,
	OPCODE_lreturn = 0xad,
	OPCODE_freturn = 0xae,
	OPCODE_dreturn = 0xaf,
	OPCODE_areturn = 0xb0,
	OPCODE_return = 0xb1,
	OPCODE_getstatic = 0xb2,
	OPCODE_putstatic = 0xb3,
	OPCODE_getfield = 0xb4,
	OPCODE_putfield = 0xb5,
	OPCODE_invokevirtual = 0xb6,
	OPCODE_invokespecial = 0xb7,
	OPCODE_invokestatic = 0xb8,
	OPCODE_invokeinterface = 0xb9,
	OPCODE_invokedynamic = 0xba,
	OPCODE_new = 0xbb,
	OPCODE_newarray = 0xbc,
	OPCODE_anewarray = 0xbd,
	OPCODE_arraylength = 0xbe,
	OPCODE_athrow = 0xbf,
	OPCODE_checkcast = 0xc0,
	OPCODE_instanceof = 0xc1,
	OPCODE_monitorenter = 0xc2,
	OPCODE_monitorexit = 0xc3,
	OPCODE_wide = 0xc4,
	OPCODE_multianewarray = 0xc5,
	OPCODE_ifnull = 0xc6,
	OPCODE_ifnonnull = 0xc7,
	OPCODE_goto_w = 0xc8,
	OPCODE_jsr_w = 0xc9,
	OPCODE_breakpoint = 0xca,
	OPCODE_impdep1 = 0xfe,
	OPCODE_impdep2 = 0xff
};

enum OpKind {
	KIND_ZERO,
	KIND_BIPUSH,
	KIND_SIPUSH,
	KIND_LDC,
	KIND_VAR,
	KIND_IINC,
	KIND_JUMP,
	KIND_TABLESWITCH,
	KIND_LOOKUPSWITCH,
	KIND_FIELD,
	KIND_INVOKE,
	KIND_INVOKEINTERFACE,
	KIND_INVOKEDYNAMIC,
	KIND_TYPE,
	KIND_NEWARRAY,
	KIND_MULTIARRAY,
	KIND_PARSE4TODO,
	KIND_RESERVED,
	KIND_LABEL,
	KIND_FRAME
};

/**
 *
 */
enum NewArrayType {
	NEWARRAYTYPE_BOOLEAN = 4,
	NEWARRAYTYPE_CHAR = 5,
	NEWARRAYTYPE_FLOAT = 6,
	NEWARRAYTYPE_DOUBLE = 7,
	NEWARRAYTYPE_BYTE = 8,
	NEWARRAYTYPE_SHORT = 9,
	NEWARRAYTYPE_INT = 10,
	NEWARRAYTYPE_LONG = 11
};

/**
 *
 */
enum AttrKind {
	ATTR_UNKNOWN,
	ATTR_SOURCEFILE,
	ATTR_CODE,
	ATTR_EXCEPTIONS,
	ATTR_LVT,
	ATTR_LVTT,
	ATTR_LNT,
	ATTR_SMT
};

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
public:

	static Type topType() {
		return Type(TYPE_TOP);
	}

	static Type intType() {
		return Type(TYPE_INTEGER, "I");
	}

	static Type floatType() {
		return Type(TYPE_FLOAT, "F");
	}

	static Type longType() {
		return Type(TYPE_LONG, "J");
	}

	static Type doubleType() {
		return Type(TYPE_DOUBLE, "D");
	}

	static Type booleanType() {
		return Type(TYPE_BOOLEAN, "Z");
	}

	static Type byteType() {
		return Type(TYPE_BYTE, "B");
	}

	static Type charType() {
		return Type(TYPE_CHAR, "C");
	}

	static Type shortType() {
		return Type(TYPE_SHORT, "S");
	}

	static Type nullType() {
		return Type(TYPE_NULL);
	}

	static Type uninitThisType() {
		return Type(TYPE_UNINITTHIS);
	}

	static Type objectType(const String& className, u2 cpindex = 0);

	static Type uninitType(short offset, class Inst* label) {
		return Type(TYPE_UNINIT, offset, label);
	}

	static Type voidType() {
		return Type(TYPE_VOID);
	}

	static Type arrayType(const Type& baseType, u4 dims);

	union {
		struct {
			short offset;
			Inst* label;
		} uninit;
	};

	bool operator==(const Type& other) const {
		return tag == other.tag
				&& (tag != TYPE_OBJECT || className == other.className)
				&& dims == other.dims;
	}

	bool isTop() const {
		return tag == TYPE_TOP && !isArray();
	}

	bool isInt() const {
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
		return isInt() || isFloat() || isNull() || isObject();
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
	Type elementType() const;

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
	Type stripArrayType() const;

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

	TypeTag tag;
	u4 dims;
	u2 classIndex;
	String className;

	Type(TypeTag tag) :
			tag(tag), dims(0), classIndex(0) {
	}

	Type(TypeTag tag, short offset, Inst* label) :
			tag(tag), dims(0), classIndex(0) {
		uninit.offset = offset;
		uninit.label = label;
	}

	Type(TypeTag tag, const String& className, u2 classIndex = 0) :
			tag(tag), dims(0), classIndex(classIndex), className(className) {
	}

	Type(const Type& other, u4 dims) :
			Type(other) {
		this->dims = dims;
	}
};

/**
 *
 */
class Frame {
public:

	Frame() :
			valid(false) {
	}

	Type pop();

	Type popOneWord();

	Type popTwoWord();

	Type popInt();

	Type popFloat();

	Type popLong();

	Type popDouble();

	Type popRef() {
		Type t = popOneWord();
		//assert(t.is(), "invalid ref type on top of the stack");
		return t;
	}

	Type popArray() {
		return popOneWord();
	}

	void popType(const Type& type);

	void pushType(const Type& type);

	void push(const Type& t) {
		stack.push_front(t);
	}

	void pushInt() {
		push(Type::intType());
	}
	void pushLong() {
		push(Type::topType());
		push(Type::longType());
	}
	void pushFloat() {
		push(Type::floatType());
	}
	void pushDouble() {
		push(Type::topType());
		push(Type::doubleType());
	}

	void pushRef(const String& className) {
		push(Type::objectType(className));
	}

	void pushArray(const Type& type, u4 dims) {
		push(Type::arrayType(type, dims));
	}

	void pushNull() {
		push(Type::nullType());
	}

	const Type& getVar(u4 lvindex) {
		return lva.at(lvindex);
	}

	void setVar(u4* lvindex, const Type& t);

	void setIntVar(u4 lvindex) {
		setVar(&lvindex, Type::intType());
	}

	void setLongVar(u4 lvindex) {
		setVar(&lvindex, Type::longType());
	}

	void setFloatVar(u4 lvindex) {
		setVar(&lvindex, Type::floatType());
	}

	void setDoubleVar(u4 lvindex) {
		setVar(&lvindex, Type::doubleType());
	}

	void setRefVar(u4 lvindex, const String& className) {
		setVar(&lvindex, Type::objectType(className));
	}

	void setRefVar(u4 lvindex, const Type& type);

	void clearStack() {
		stack.clear();
	}

	void cleanTops();

	std::vector<Type> lva;
	std::list<Type> stack;
	bool valid;

private:

	void _setVar(u4 lvindex, const Type& t);

};

/**
 * Represent a bytecode instruction.
 */
class Inst {
	friend class LabelInst;
	friend class ZeroInst;
	friend class PushInst;
	friend class LdcInst;
	friend class VarInst;
	friend class IincInst;
	friend class WideInst;
	friend class JumpInst;
	friend class FieldInst;
	friend class InvokeInst;
	friend class InvokeInterfaceInst;
	friend class TypeInst;
	friend class NewArrayInst;
	friend class MultiArrayInst;
	friend class TableSwitchInst;
	friend class LookupSwitchInst;
	friend class InstList;

public:

	bool isJump() const {
		return kind == KIND_JUMP;
	}

	bool isTableSwitch() const {
		return kind == KIND_TABLESWITCH;
	}

	bool isLookupSwitch() const {
		return kind == KIND_LOOKUPSWITCH;
	}

	bool isBranch() const {
		return isJump() || isTableSwitch() || isLookupSwitch();
	}

	bool isExit() const {
		return (opcode >= OPCODE_ireturn && opcode <= OPCODE_return)
				|| opcode == OPCODE_athrow;
	}

	bool isLabel() const {
		return kind == KIND_LABEL;
	}

	bool isPush() const {
		return kind == KIND_BIPUSH || kind == KIND_SIPUSH;
	}

	bool isLdc() const {
		return kind == KIND_LDC;
	}

	bool isVar() const {
		return kind == KIND_VAR;
	}

	bool isIinc() const {
		return kind == KIND_IINC;
	}

	bool isInvoke() const {
		return kind == KIND_INVOKE;
	}
	bool isInvokeInterface() const {
		return kind == KIND_INVOKEINTERFACE;
	}
	bool isType() const {
		return kind == KIND_TYPE;
	}
	bool isNewArray() const {
		return kind == KIND_NEWARRAY;
	}
	bool isWide() const {
		return opcode == OPCODE_wide && kind == KIND_ZERO;
	}
	bool isField() const {
		return kind == KIND_FIELD;
	}
	bool isMultiArray() const {
		return kind == KIND_MULTIARRAY;
	}

	bool isJsrOrRet() const {
		return opcode == OPCODE_jsr || opcode == OPCODE_jsr_w
				|| opcode == OPCODE_ret;
	}

	/**
	 * The opcode of this instruction.
	 */
	const Opcode opcode;

	/**
	 * The kind of this instruction.
	 */
	const OpKind kind;

	int _offset;

	const ConstPool* const constPool;
	Inst* prev;
	Inst* next;

	/**
	 *
	 */
	class LabelInst* label() {
		return cast<LabelInst>(isLabel(), "label");
	}

	class PushInst* push() {
		return cast<PushInst>(isPush(), "push");
	}

	class LdcInst* ldc() {
		return cast<LdcInst>(isLdc(), "ldc");
	}

	class VarInst* var() {
		return cast<VarInst>(isVar(), "var");
	}

	class IincInst* iinc() {
		return cast<IincInst>(isIinc(), "iinc");
	}

	class InvokeInst* invoke() {
		return cast<InvokeInst>(isInvoke(), "invoke");
	}

	class JumpInst* jump() {
		return cast<JumpInst>(isJump(), "jump");
	}

	class TableSwitchInst* ts() {
		return cast<TableSwitchInst>(isTableSwitch(), "ts");
	}

	class LookupSwitchInst* ls() {
		return cast<LookupSwitchInst>(isLookupSwitch(), "ls");
	}

	class InvokeInterfaceInst* invokeinterface() {
		return cast<InvokeInterfaceInst>(isInvokeInterface(), "invinter");
	}

	class TypeInst* type() {
		return cast<TypeInst>(isType(), "type");
	}

	class NewArrayInst* newarray() {
		return cast<NewArrayInst>(isNewArray(), "newarray");
	}

	class WideInst* wide() {
		return cast<WideInst>(isWide(), "wide");
	}

	class FieldInst* field() {
		return cast<FieldInst>(isField(), "field");
	}

	class MultiArrayInst* multiarray() {
		return cast<MultiArrayInst>(isMultiArray(), "multiarray");
	}

private:

	Inst() :
			opcode(OPCODE_nop), kind(KIND_ZERO), _offset(0), constPool(nullptr), prev(
					nullptr), next(nullptr) {
	}

	Inst(Opcode opcode, OpKind kind, ConstPool* constPool, Inst* prev = nullptr,
			Inst* next = nullptr) :
			opcode(opcode), kind(kind), _offset(0), constPool(constPool), prev(
					prev), next(next) {
	}

	template<typename TKind>
	TKind* cast(bool cond, const char* kindName) {
		checkCast(cond, kindName);
		return (TKind*) this;
	}

	void checkCast(bool cond, const char* kindName) const;
};

/**
 *
 */
class LabelInst: public Inst {
	friend class InstList;
public:

	u2 offset;
	u2 deltaOffset;
	int id;
	bool isBranchTarget;
	bool isTryStart;
	bool isCatchHandler;

private:

	LabelInst(ConstPool* constPool) :
			Inst(OPCODE_nop, KIND_LABEL, constPool), offset(0), deltaOffset(0), id(
					0), isBranchTarget(false), isTryStart(false), isCatchHandler(
					false) {
	}

};

/**
 *
 */
class ZeroInst: public Inst {
	friend class InstList;

private:

	ZeroInst(Opcode opcode, ConstPool* constPool) :
			Inst(opcode, KIND_ZERO, constPool) {
	}
};

/**
 *
 */
class PushInst: public Inst {
	friend class InstList;

public:
	int value;

private:

	PushInst(Opcode opcode, OpKind kind, int value, ConstPool* constPool) :
			Inst(opcode, kind, constPool), value(value) {
	}
};

/**
 *
 */
class LdcInst: public Inst {
	friend class InstList;

public:
	ConstIndex valueIndex;

private:

	LdcInst(Opcode opcode, ConstIndex valueIndex, ConstPool* constPool) :
			Inst(opcode, KIND_LDC, constPool), valueIndex(valueIndex) {
	}
};

/**
 *
 */
class VarInst: public Inst {
	friend class InstList;

public:
	u1 lvindex;

private:

	VarInst(Opcode opcode, u1 lvindex, ConstPool* constPool) :
			Inst(opcode, KIND_VAR, constPool), lvindex(lvindex) {
	}
};

/**
 *
 */
class IincInst: public Inst {
	friend class InstList;

public:
	u1 index;
	u1 value;

private:

	IincInst(u1 index, u1 value, ConstPool* constPool) :
			Inst(OPCODE_iinc, KIND_IINC, constPool), index(index), value(value) {
	}

};

/**
 *
 */
class WideInst: public Inst {
	friend class InstList;

public:
	Opcode subOpcode;

	union {
		struct {
			u2 lvindex;
		} var;
		struct {
			u2 index;
			u2 value;
		} iinc;
	};

private:

	WideInst(Opcode subOpcode, u2 lvindex, ConstPool* constPool) :
			Inst(OPCODE_wide, KIND_ZERO, constPool), subOpcode(subOpcode), var(
					{ lvindex }) {
	}

	WideInst(u2 index, u2 value, ConstPool* constPool) :
			Inst(OPCODE_wide, KIND_ZERO, constPool), subOpcode(OPCODE_iinc), iinc(
					{ index, value }) {
	}

};

/**
 *
 */
class JumpInst: public Inst {
	friend class InstList;

public:

	Inst* label2;

private:

	JumpInst(Opcode opcode, LabelInst* targetLabel, ConstPool* constPool) :
			Inst(opcode, KIND_JUMP, constPool), label2(targetLabel) {
	}
};

/**
 *
 */
class FieldInst: public Inst {
	friend class InstList;

public:
	ConstIndex fieldRefIndex;

private:

	FieldInst(Opcode opcode, ConstIndex fieldRefIndex, ConstPool* constPool) :
			Inst(opcode, KIND_FIELD, constPool), fieldRefIndex(fieldRefIndex) {
	}
};

/**
 *
 */
class InvokeInst: public Inst {
	friend class InstList;

public:
	ConstIndex methodRefIndex;

private:

	InvokeInst(Opcode opcode, ConstIndex methodRefIndex, ConstPool* constPool) :
			Inst(opcode, KIND_INVOKE, constPool), methodRefIndex(methodRefIndex) {
	}
};

/**
 *
 */
class InvokeInterfaceInst: public Inst {
	friend class InstList;

public:
	u2 interMethodRefIndex;
	u1 count;

private:

	InvokeInterfaceInst(ConstIndex interMethodRefIndex, u1 count,
			ConstPool* constPool) :
			Inst(OPCODE_invokeinterface, KIND_INVOKEINTERFACE, constPool), interMethodRefIndex(
					interMethodRefIndex), count(count) {
	}
};

/**
 *
 */
class TypeInst: public Inst {
	friend class InstList;

public:

	/**
	 * Index in the constant pool of a class entry.
	 */
	ConstIndex classIndex;

private:

	TypeInst(Opcode opcode, ConstIndex classIndex, ConstPool* constPool) :
			Inst(opcode, KIND_TYPE, constPool), classIndex(classIndex) {
	}

};

/**
 *
 */
class NewArrayInst: public Inst {
	friend class InstList;

public:
	u1 atype;

private:

	NewArrayInst(Opcode opcode, u1 atype, ConstPool* constPool) :
			Inst(opcode, KIND_NEWARRAY, constPool), atype(atype) {
	}

};

/**
 *
 */
class MultiArrayInst: public Inst {
	friend class InstList;

public:
	ConstIndex classIndex;
	u1 dims;

private:

	MultiArrayInst(Opcode opcode, ConstIndex classIndex, u1 dims,
			ConstPool* constPool) :
			Inst(opcode, KIND_MULTIARRAY, constPool), classIndex(classIndex), dims(
					dims) {
	}

};

/**
 *
 */
class TableSwitchInst: public Inst {
	friend class InstList;

public:

	Inst* def;
	int low;
	int high;
	std::vector<Inst*> targets;

private:

	TableSwitchInst(LabelInst* def, int low, int high, ConstPool* constPool) :
			Inst(OPCODE_tableswitch, KIND_TABLESWITCH, constPool), def(def), low(
					low), high(high) {
	}

};

/**
 *
 */
class LookupSwitchInst: public Inst {
	friend class InstList;

public:
	Inst* defbyte;
	u4 npairs;
	std::vector<u4> keys;
	std::vector<Inst*> targets;

private:

	LookupSwitchInst(LabelInst* def, u4 npairs, ConstPool* constPool) :
			Inst(OPCODE_lookupswitch, KIND_LOOKUPSWITCH, constPool), defbyte(
					def), npairs(npairs) {
	}
};

/**
 * Represents the bytecode of a method.
 */
//
class InstList {
	friend class CodeAttr;

public:

	class Iterator {
		friend InstList;
	public:

		Inst* operator*();

		Inst* operator->() const;

		bool friend operator==(const Iterator& lhs, const Iterator& rhs) {
			return lhs.position == rhs.position;
		}

		bool friend operator!=(const Iterator& lhs, const Iterator& rhs) {
			return lhs.position != rhs.position;
		}

		Iterator& operator++();

		Iterator& operator--();

	private:

		Iterator(Inst* position, Inst* last) :
				position(position), last(last) {
		}

		Inst* position;
		Inst* last;
	};

	LabelInst* createLabel() const {
		auto inst = new LabelInst(constPool);
		return inst;
	}

	void addLabel(LabelInst* inst, Inst* pos = nullptr) {
		//auto inst = new LabelInst(constPool);
		addInst(inst, pos);
	}

	void addLabel(Inst* pos = nullptr) {
		auto inst = createLabel();
		addInst(inst, pos);
	}

	void addZero(Opcode opcode, Inst* pos = nullptr) {
		auto inst = new ZeroInst(opcode, constPool);
		addInst(inst, pos);
	}

	void addBiPush(u1 value, Inst* pos = nullptr) {
		auto inst = new PushInst(OPCODE_bipush, KIND_BIPUSH, value, constPool);
		addInst(inst, pos);
	}

	void addSiPush(u2 value, Inst* pos = nullptr) {
		auto inst = new PushInst(OPCODE_sipush, KIND_SIPUSH, value, constPool);
		addInst(inst, pos);
	}

	void addLdc(Opcode opcode, ConstIndex valueIndex, Inst* pos = nullptr) {
		auto inst = new LdcInst(opcode, valueIndex, constPool);
		addInst(inst, pos);
	}

	void addVar(Opcode opcode, u1 lvindex, Inst* pos = nullptr) {
		auto inst = new VarInst(opcode, lvindex, constPool);
		addInst(inst, pos);
	}

	void addIinc(u1 index, u1 value, Inst* pos = nullptr) {
		auto inst = new IincInst(index, value, constPool);
		addInst(inst, pos);
	}

	void addWideVar(Opcode varOpcode, u2 lvindex, Inst* pos = nullptr) {
		auto inst = new WideInst(varOpcode, lvindex, constPool);
		addInst(inst, pos);
	}

	void addWideIinc(u1 index, u1 value, Inst* pos = nullptr) {
		auto inst = new WideInst(index, value, constPool);
		addInst(inst, pos);
	}

	void addJump(Opcode opcode, LabelInst* targetLabel, Inst* pos = nullptr) {
		auto inst = new JumpInst(opcode, targetLabel, constPool);
		addInst(inst, pos);
	}

	void addField(Opcode opcode, ConstIndex fieldRefIndex,
			Inst* pos = nullptr) {
		auto inst = new FieldInst(opcode, fieldRefIndex, constPool);
		addInst(inst, pos);
	}

	void addInvoke(Opcode opcode, ConstIndex methodRefIndex,
			Inst* pos = nullptr) {
		auto inst = new InvokeInst(opcode, methodRefIndex, constPool);
		addInst(inst, pos);
	}

	void addInvokeInterface(ConstIndex interMethodRefIndex, u1 count,
			Inst* pos = nullptr) {
		auto inst = new InvokeInterfaceInst(interMethodRefIndex, count,
				constPool);
		addInst(inst, pos);
	}

	void addType(Opcode opcode, ConstIndex classIndex, Inst* pos = nullptr) {
		auto inst = new TypeInst(opcode, classIndex, constPool);
		addInst(inst, pos);
	}

	void addNewArray(u1 atype, Inst* pos = nullptr) {
		auto inst = new NewArrayInst(OPCODE_newarray, atype, constPool);
		addInst(inst, pos);
	}

	void addMultiArray(ConstIndex classIndex, u1 dims, Inst* pos = nullptr) {
		auto inst = new MultiArrayInst(OPCODE_multianewarray, classIndex, dims,
				constPool);
		addInst(inst, pos);
	}

	TableSwitchInst* addTableSwitch(LabelInst* def, int low, int high,
			Inst* pos = nullptr) {
		auto inst = new TableSwitchInst(def, low, high, constPool);
		addInst(inst, pos);

		return inst;
	}

	LookupSwitchInst* addLookupSwitch(LabelInst* def, u4 npairs, Inst* pos =
			nullptr) {
		auto inst = new LookupSwitchInst(def, npairs, constPool);
		addInst(inst, pos);

		return inst;
	}

	void setLabelIds() {
		int id = 1;
		for (Inst* inst : *this) {
			if (inst->isLabel()) {
				inst->label()->id = id;
				id++;
			}
		}
	}

	bool hasBranches() const {
		for (const Inst* inst : *this) {
			if (inst->isBranch()) {
				return true;
			}
		}

		return false;
	}

	Iterator begin() const {
		return Iterator(first, last);
	}

	Iterator end() const {
		return Iterator(nullptr, last);
	}

	ConstPool* const constPool;

private:

	InstList(ConstPool* constPool) :
			constPool(constPool), first(nullptr), last(nullptr), size(0) {
	}

	~InstList() {
		for (Inst* inst : *this) {
			delete inst;
		}
	}

	void addInst(Inst* inst, Inst* pos);

	Inst* first;
	Inst* last;

	int size;
};

class ControlFlowGraph;

/**
 * Represents a basic block of instructions.
 *
 * @see Inst
 */
class BasicBlock {
public:
	BasicBlock(const BasicBlock&) = delete;
	BasicBlock(BasicBlock&&) = default;

	friend ControlFlowGraph;

	void addTarget(BasicBlock* target);

	InstList::Iterator start;
	InstList::Iterator exit;
	String name;
	Frame in;
	Frame out;

	std::vector<BasicBlock*>::iterator begin() {
		return targets.begin();
	}

	std::vector<BasicBlock*>::iterator end() {
		return targets.end();
	}

	BasicBlock* next;
	ControlFlowGraph* cfg;

private:

	BasicBlock(InstList::Iterator& start, InstList::Iterator& exit,
			const String& name, ControlFlowGraph* cfg) :
			start(start), exit(exit), name(name), next(nullptr), cfg(cfg) {
	}

	std::vector<BasicBlock*> targets;
};

/**
 * Represents a control flow graph of instructions.
 */
class ControlFlowGraph {
private:
	std::vector<BasicBlock*> basicBlocks;

public:
	BasicBlock* const entry;

	BasicBlock* const exit;

	const InstList& instList;

	ControlFlowGraph(InstList& instList);

	~ControlFlowGraph() {
		for (auto bb : *this) {
			delete bb;
		}
	}

	/**
	 * Adds a basic block to this control flow graph.
	 *
	 * @param start the start of the basic block.
	 * @param end the end of the basic block.
	 * @param name the name of the basic block to add.
	 * @returns the newly created basic block added to this control flow graph.
	 */
	BasicBlock* addBasicBlock(InstList::Iterator start, InstList::Iterator end,
			const String& name);

	/**
	 * Finds the basic block associated with the given labelId.
	 *
	 * @param labelId the label id to search in the basic blocks.
	 * @returns the basic block that starts at label with labelId. If the label
	 * id is not found throws an exception.
	 */
	BasicBlock* findBasicBlockOfLabel(int labelId) const;

	std::vector<BasicBlock*>::iterator begin() {
		return basicBlocks.begin();
	}

	std::vector<BasicBlock*>::iterator end() {
		return basicBlocks.end();
	}

	std::vector<BasicBlock*>::const_iterator begin() const {
		return basicBlocks.begin();
	}

	std::vector<BasicBlock*>::const_iterator end() const {
		return basicBlocks.end();
	}

private:
	BasicBlock* addConstBb(InstList& instList, const char* name) {
		return addBasicBlock(instList.end(), instList.end(), name);
	}

};

/**
 * Defines the base class for all attributes in the class file.
 */
class Attr {

	Attr(const Attr&) = delete;
public:

	AttrKind kind;

	u2 nameIndex;
	u4 len;
	ConstPool* const constPool;

//	virtual void accept(Visitor* v) = 0;

protected:

	Attr(AttrKind kind, u2 nameIndex, u4 len, ConstPool* constPool) :
			kind(kind), nameIndex(nameIndex), len(len), constPool(constPool) {
	}
};

/**
 * Represents a collection of attributes within a class, method or field
 * or even with another attributes, e.g., CodeAttr.
 */
struct Attrs {
	Attrs(const Attrs&) = delete;
	Attrs(Attrs&&) = default;

	Attrs() {
	}

	~Attrs() {
		for (Attr* attr : attrs) {
			delete attr;
		}
	}

	Attr* add(Attr* attr) {
		attrs.push_back(attr);

		return attr;
	}

	u2 size() const {
		return attrs.size();
	}

	const Attr& operator[](u2 index) const {
		return *attrs[index];
	}

	std::vector<Attr*>::iterator begin() {
		return attrs.begin();
	}

	std::vector<Attr*>::iterator end() {
		return attrs.end();
	}

	std::vector<Attr*> attrs;
};

/**
 * Represents an unknown opaque attribute to jnif.
 */
class UnknownAttr: public Attr {
public:

	const u1 * const data;

	UnknownAttr(u2 nameIndex, u4 len, const u1* data, ConstPool* constPool) :
			Attr(ATTR_UNKNOWN, nameIndex, len, constPool), data(data) {
	}

//	void accept(Visitor* v) {
//		v->visit(*this);
//	}
};

/**
 * Represents the LineNumberTable attribute within the Code attribute.
 */
class LvtAttr: public Attr {
public:

	struct LvEntry {
		u2 startPc;
		Inst* startPcLabel;

		u2 len;
		u2 varNameIndex;
		u2 varDescIndex;
		u2 index;
	};

	std::vector<LvEntry> lvt;

	LvtAttr(AttrKind kind, u2 nameIndex, ConstPool* constPool) :
			Attr(kind, nameIndex, 0, constPool) {
	}
};

/**
 * Represents the LineNumberTable attribute within the Code attribute.
 */
class LntAttr: public Attr {
public:

	LntAttr(u2 nameIndex, ConstPool* constPool) :
			Attr(ATTR_LNT, nameIndex, 0, constPool) {
	}

	struct LnEntry {
		u2 startpc;
		Inst* startPcLabel;

		u2 lineno;
	};

	std::vector<LnEntry> lnt;

};

/**
 *
 */
class SmtAttr: public Attr {
public:

	SmtAttr(u2 nameIndex, ConstPool* constPool) :
			Attr(ATTR_SMT, nameIndex, 0, constPool) {
	}

	class Entry {
	public:

		int frameType;
		Inst* label;

		struct {
		} sameFrame;
		struct {
			std::vector<Type> stack; // [1]
		} sameLocals_1_stack_item_frame;
		struct {
			short offset_delta;
			std::vector<Type> stack; // [1]
		} same_locals_1_stack_item_frame_extended;
		struct {
			short offset_delta;
		} chop_frame;
		struct {
			short offset_delta;
		} same_frame_extended;
		struct {
			short offset_delta;
			std::vector<Type> locals; // frameType - 251
		} append_frame;
		struct {
			short offset_delta;
			std::vector<Type> locals;
			std::vector<Type> stack;
		} full_frame;
	};

	std::vector<Entry> entries;
};

/**
 * Represents the Exceptions attribute.
 */
class ExceptionsAttr: public Attr {
public:

	ExceptionsAttr(u2 nameIndex, ConstPool* constPool,
			const std::vector<u2>& es) :
			Attr(ATTR_EXCEPTIONS, nameIndex, es.size() * 2 + 2, constPool), es(
					es) {
	}

	std::vector<ConstIndex> es;
};

/**
 *
 */
struct CodeExceptionEntry {
	Inst* startpc;
	Inst* endpc;
	Inst* handlerpc;
	ConstIndex catchtype;
};

/**
 * Represent the Code attribute of a method.
 */
class CodeAttr: public Attr {
public:

	CodeAttr(u2 nameIndex, ConstPool* constPool) :
			Attr(ATTR_CODE, nameIndex, 0, constPool), maxStack(0), maxLocals(0), codeLen(
					0), instList(constPool), cfg(nullptr) {
	}

	u2 maxStack;
	u2 maxLocals;
	u4 codeLen;

	InstList instList;

	bool hasTryCatch() const {
		return exceptions.size() > 0;
	}

	std::vector<CodeExceptionEntry> exceptions;

	ControlFlowGraph* cfg;

	Attrs attrs;
};

/**
 *
 */
class SourceFileAttr: public Attr {
public:

	const ConstIndex sourceFileIndex;

	SourceFileAttr(ConstIndex nameIndex, ConstIndex sourceFileIndex,
			ConstPool* constPool) :
			Attr(ATTR_SOURCEFILE, nameIndex, 2, constPool), sourceFileIndex(
					sourceFileIndex) {
	}

};

/**
 * Represent a member of a class. This the base class for Field and
 * Method classes.
 *
 * @see Field
 * @see Method
 */
class Member: public Attrs {
public:

	friend class Field;
	friend class Method;
	Member(const Member&) = delete;
	Member(Member&&) = default;

	u2 accessFlags;
	ConstIndex nameIndex;
	ConstIndex descIndex;
	ConstPool* const constPool;

	std::string getName() const {
		std::string name = constPool->getUtf8(nameIndex);
		return name;
	}

private:

	Member(u2 accessFlags, ConstIndex nameIndex, ConstIndex descIndex,
			ConstPool* constPool) :
			accessFlags(accessFlags), nameIndex(nameIndex), descIndex(
					descIndex), constPool(constPool) {
	}
};

/**
 *
 */
class Field: public Member {
	friend class ClassFile;

public:

	Field(u2 accessFlags, ConstIndex nameIndex, ConstIndex descIndex,
			ConstPool* constPool) :
			Member(accessFlags, nameIndex, descIndex, constPool) {
	}

};

/**
 *
 */
class Method: public Member {
	friend class ClassFile;

public:

	bool hasCode() const {
		for (Attr* attr : attrs) {
			if (attr->kind == ATTR_CODE) {
				return true;
			}
		}

		return false;
	}

	CodeAttr* codeAttr() const {
		for (Attr* attr : attrs) {
			if (attr->kind == ATTR_CODE) {
				return (CodeAttr*) attr;
			}
		}

		return nullptr;
	}

	InstList& instList();

//	void instList(const InstList& newcode) {
//		for (Attr* attr : attrs) {
//			if (attr->kind == ATTR_CODE) {
//				((CodeAttr*) attr)->instList = newcode;
//				return;
//			}
//		}
//
//		Error::raise("ERROR! setting inst list");
//	}

	bool isStatic() const {
		return accessFlags & METHOD_STATIC;
	}

private:

	Method(u2 accessFlags, ConstIndex nameIndex, ConstIndex descIndex,
			ConstPool* constPool) :
			Member(accessFlags, nameIndex, descIndex, constPool) {
	}

};

class IClassPath {
public:

	virtual ~IClassPath() {
	}

	virtual String getCommonSuperClass(const String& className1,
			const String& className2) = 0;

};

/**
 * The Version class is a tuple containing a major and minor
 * version numbers fields.
 *
 * The values of the major and minor fields are the minor
 * and major version numbers of this class file.
 * Together, a major and a minor version number determine the version of the
 * class file format.
 * If a class file has major version number M and minor version number m,
 * we denote the version of its class file format as M.m.
 * Thus, class file format versions may be ordered lexicographically,
 * for example, 1.5 < 2.0 < 2.1.
 *
 * A Java Virtual Machine implementation can support a class file format of
 * version v if and only if v lies in some contiguous range Mi.0 <= v <= Mj.m.
 * The release level of the Java SE platform to which a Java Virtual Machine
 * implementation conforms is responsible for determining the range.
 */
class Version {
public:

	/**
	 * Using default 51, which is supported by JDK 1.7.
	 */
	Version(u2 major = 51, u2 minor = 0) :
			major(major), minor(minor) {
	}

	/**
	 * The major version number.
	 */
	u2 major;

	/**
	 * The minor version number.
	 */
	u2 minor;

	/**
	 *
	 */
	friend bool operator==(const Version& left, const Version& right) {
		return left.major == right.major && left.major == right.major;
	}

	/**
	 *
	 */
	friend bool operator<(const Version& left, const Version& right) {
		return left.major < right.major
				|| (left.major == right.major && left.minor < right.minor);
	}

	/**
	 *
	 */
	friend bool operator<=(const Version& left, const Version& right) {
		return left < right || left == right;
	}

	/**
	 * Taken from the oficial JVM specification.
	 *
	 * Oracle's Java Virtual Machine implementation in JDK release 1.0.2
	 * supports class file format versions 45.0 through 45.3 inclusive.
	 * JDK releases 1.1.* support class file format versions in the
	 * range 45.0 through 45.65535 inclusive.
	 * For k >= 2, JDK release 1.k supports class file format versions in
	 * the range 45.0 through 44+k.0 inclusive.
	 */
	String supportedByJdk() const;
};

/**
 * Models a Java Class File following the specification of the JVM version 7.
 */
class ClassFile: public ConstPool, public Attrs {
public:

	/**
	 * Constructs a default class file given the class name, the super class
	 * name and the access flags.
	 */
	ClassFile(const char* className, const char* superClassName =
			"java/lang/Object", u2 accessFlags = CLASS_PUBLIC, u2 majorVersion =
			51, u2 minorVersion = 0) :
			version(majorVersion, minorVersion), accessFlags(accessFlags), thisClassIndex(
					addClass(className)), superClassIndex(
					addClass(superClassName)) {
	}

	/**
	 * Constructs a class file from an in-memory representation of the java
	 * class file.
	 */
	ClassFile(const u1* classFileData, int classFileLen);

	/**
	 * Gets the class name of this class file.
	 */
	const char* getThisClassName() const {
		return getClassName(thisClassIndex);
	}

	/**
	 *
	 */
	Field* addField(ConstIndex nameIndex, ConstIndex descIndex, u2 accessFlags =
			FIELD_PUBLIC) {
		Field* field = new Field(accessFlags, nameIndex, descIndex, this);
		fields.push_back(field);
		return field;
	}

	/**
	 *
	 */
	Field* addField(const char* fieldName, const char* fieldDesc,
			u2 accessFlags = FIELD_PUBLIC) {
		ConstIndex nameIndex = addUtf8(fieldName);
		ConstIndex descIndex = addUtf8(fieldDesc);

		return addField(nameIndex, descIndex, accessFlags);
	}

	/**
	 *
	 */
	Method* addMethod(ConstIndex nameIndex, ConstIndex descIndex,
			u2 accessFlags = METHOD_PUBLIC) {
		Method* method = new Method(accessFlags, nameIndex, descIndex, this);
		methods.push_back(method);
		return method;
	}

	/**
	 *
	 */
	Method* addMethod(const char* methodName, const char* methodDesc,
			u2 accessFlags = METHOD_PUBLIC) {
		ConstIndex nameIndex = addUtf8(methodName);
		ConstIndex descIndex = addUtf8(methodDesc);

		return addMethod(nameIndex, descIndex, accessFlags);
	}

	/**
	 * Computes the size in bytes of this class file of the in-memory
	 * representation.
	 */
	u4 computeSize();

	/**
	 *
	 */
	void computeFrames(IClassPath* classPath);

	/**
	 * Writes this class file in the specified buffer according to the
	 * specification.
	 */
	void write(u1* classFileData, int classFileLen);

	/**
	 *
	 */
	template<typename TAllocFunc>
	void write(u1** classFileData, int* classFileSize, TAllocFunc allocFunc) {
		*classFileSize = computeSize();
		*classFileData = allocFunc(*classFileSize);
		write(*classFileData, *classFileSize);
	}

	/**
	 * Export this class file to dot format.
	 *
	 * @see www.graphviz.org
	 */
	void dot(std::ostream& os) const;

	Version version;
	u2 accessFlags;
	ConstIndex thisClassIndex;
	ConstIndex superClassIndex;
	std::vector<ConstIndex> interfaces;
	std::vector<Field*> fields;
	std::vector<Method*> methods;
};

/**
 *
 */
class IClassFinder {
public:

	virtual ~IClassFinder() {
	}

	virtual ClassFile* findClass(const String& className) = 0;

};

/**
 *
 */
class ClassHierarchy {
public:

//	IClassFinder* finder;
//
//	ClassHierarchy(IClassFinder* finder) :
//			finder(finder) {
//	}

	/**
	 *
	 */
	class ClassEntry {
	public:
		String className;
		String superClassName;
		std::vector<String> interfaces;
	};

	/**
	 *
	 */
	//ClassFile* defineClass(const ClassFile& classFile);
	/**
	 *
	 */
	void addClass(const ClassFile& classFile);

	const String& getSuperClass(const String& className) const;

	bool isAssignableFrom(const String& sub, const String& sup) const;

	bool isDefined(const String& className) const;

	std::list<ClassEntry>::iterator begin() {
		return classes.begin();
	}

	std::list<ClassEntry>::iterator end() {
		return classes.end();
	}

	std::list<ClassEntry>::const_iterator begin() const {
		return classes.begin();
	}

	std::list<ClassEntry>::const_iterator end() const {
		return classes.end();
	}

private:

	std::list<ClassEntry> classes;

	const ClassEntry* getEntry(const String& className) const;
};

std::ostream& operator<<(std::ostream& os, const JnifException& ex);
std::ostream& operator<<(std::ostream& os, const ConstTag& tag);
std::ostream& operator<<(std::ostream& os, const Frame& frame);
std::ostream& operator<<(std::ostream& os, const Type& type);
std::ostream& operator<<(std::ostream& os, Inst& inst);
std::ostream& operator<<(std::ostream& os, InstList& instList);
std::ostream& operator<<(std::ostream& os, BasicBlock& bb);
std::ostream& operator<<(std::ostream& os, const ControlFlowGraph& cfg);
std::ostream& operator<<(std::ostream& os, const Version& version);
std::ostream& operator<<(std::ostream& os, ClassFile& classFile);
std::ostream& operator<<(std::ostream& os, const ClassHierarchy& ch);

}

#endif
