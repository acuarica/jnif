/*
 * ClassFile.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_CLASSFILE_HPP
#define JNIF_CLASSFILE_HPP

#include "ConstPool.hpp"
#include "Version.hpp"
#include "CodeAttr.hpp"
#include "Inst.hpp"

#include "Arena.hpp"

namespace jnif {

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
	ClassFile* const constPool;

	String getName() const;

private:

	Member(u2 accessFlags, ConstIndex nameIndex, ConstIndex descIndex,
			ClassFile* constPool) :
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
			ClassFile* constPool) :
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

		return NULL;
	}

	InstList& instList();

	bool isPublic() const {
		return accessFlags & METHOD_PUBLIC;
	}

	bool isStatic() const {
		return accessFlags & METHOD_STATIC;
	}

	bool isInit() const;

	bool isMain() const;

	/**
	 * Shows this method in the specified ostream.
	 */
	friend std::ostream& operator<<(std::ostream& os, const Method& m);

private:

	Method(u2 accessFlags, ConstIndex nameIndex, ConstIndex descIndex,
			ClassFile* constPool) :
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
 * The magic number signature that must appear at the beginning of each
 * class file, identifying the class file format; it has the value 0xCAFEBABE.
 */
enum Magic {
	CLASSFILE_MAGIC = 0xcafebabe
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
	 * Releases the memory used for this class file.
	 */
	~ClassFile();

	/**
	 * Gets the class name of this class file.
	 */
	const char* getThisClassName() const {
		return getClassName(thisClassIndex);
	}

	/**
	 *
	 */
	String getSuperClassName() const {
		return getClassName(superClassIndex);
	}

	/**
	 * Adds a new field to this class file.
	 *
	 * @param nameIndex the utf8 index in the constant pool that contains the
	 * name of the field to add.
	 * @param descIndex the utf8 index in the constant pool that contains the
	 * descriptor of the field to add.
	 * @param accessFlags the access flags of the field to add.
	 * @returns the newly created field.
	 */
	Field* addField(ConstIndex nameIndex, ConstIndex descIndex, u2 accessFlags =
			FIELD_PUBLIC);

	/**
	 * Adds a new field to this class file by passing directly the name
	 * and descriptor.
	 *
	 * @param fieldName the name of the field to add.
	 * @param fieldDesc the descriptor of the field to add.
	 * @param accessFlags the access flags of the field to add.
	 * @returns the newly created field.
	 */
	Field* addField(const char* fieldName, const char* fieldDesc,
			u2 accessFlags = FIELD_PUBLIC) {
		ConstIndex nameIndex = addUtf8(fieldName);
		ConstIndex descIndex = addUtf8(fieldDesc);

		return addField(nameIndex, descIndex, accessFlags);
	}

	/**
	 * Adds a new method to this class file.
	 *
	 * @param nameIndex the utf8 index in the constant pool that contains the
	 * name of the method to add.
	 * @param descIndex the utf8 index in the constant pool that contains the
	 * descriptor of the method to add.
	 * @param accessFlags the access flags of the field to add.
	 * @returns the newly created method.
	 */
	Method* addMethod(ConstIndex nameIndex, ConstIndex descIndex,
			u2 accessFlags = METHOD_PUBLIC);

	/**
	 * Adds a new method to this class file by passing directly the name
	 * and descriptor.
	 *
	 * @param methodName the name of the method to add.
	 * @param methodDesc the descriptor of the method to add.
	 * @param accessFlags the access flags of the method to add.
	 * @returns the newly created method.
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
//	template<typename TAllocFunc>
//	void write(u1** classFileData, int* classFileSize, TAllocFunc allocFunc) {
//		*classFileSize = computeSize();
//		*classFileData = allocFunc(*classFileSize);
//		write(*classFileData, *classFileSize);
//	}
	/**
	 * Export this class file to dot format.
	 *
	 * @see www.graphviz.org
	 */
	void dot(std::ostream& os) const;

	Arena _arena;

	Version version;
	u2 accessFlags;
	ConstIndex thisClassIndex;
	ConstIndex superClassIndex;
	std::vector<ConstIndex> interfaces;
	std::vector<Field*> fields;
	std::vector<Method*> methods;
};

std::ostream& operator<<(std::ostream& os, const ClassFile& classFile);

}

#endif
