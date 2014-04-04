#include "jnif.hpp"
#include "jniferr.hpp"

#include "ClassParser.hpp"
#include "ClassWriter.hpp"
#include "buffer/SizeWriter.hpp"
#include "buffer/BufferWriter.hpp"

namespace jnif {

InstList& Member::instList() {
	for (Attr* attr : attrs) {
		if (attr->kind == ATTR_CODE) {
			return ((CodeAttr*) attr)->instList;
		}
	}

	EXCEPTION("ERROR! get inst list");
}

void Member::instList(const InstList& newcode) {
	for (Attr* attr : attrs) {
		if (attr->kind == ATTR_CODE) {
			((CodeAttr*) attr)->instList = newcode;
			return;
		}
	}

	EXCEPTION("ERROR! setting inst list");
}

u2 Version::getMajor() const {
	return _major;
}

u2 Version::getMinor() const {
	return _minor;
}

//void parseClassFile(const u1* fileImage, const int fileImageLen, ClassFile& cf);

typedef ClassParser<BufferReader<ExceptionManager>> ClsParser;

u4 getClassFileSize(ClassFile& cf);
void writeClassFile(ClassFile& cf, u1* fileImage, const int fileImageLen);
void printClassFile(ClassFile& cf, ostream& os, int tabs = 0);

u4 getClassFileSize(ClassFile & cf) {
	SizeWriter bw;
	ClassWriter<SizeWriter>(bw).writeClassFile(cf);

	return bw.getOffset();
}

void writeClassFile(ClassFile& cf, u1* fileImage, const int fileImageLen) {
	BufferWriter<ExceptionManager> bw(fileImage, fileImageLen);
	ClassWriter<BufferWriter<ExceptionManager>>(bw).writeClassFile(cf);
}

ClassFile::ClassFile(const char* className, const char* superClassName,
		u2 accessFlags, Version version) :
		accessFlags(accessFlags), _version(version), thisClassIndex(
				addClass(className)), superClassIndex(addClass(superClassName)) {
}

ClassFile::ClassFile(const u1* classFileData, const int classFileLen) :
		thisClassIndex(0), superClassIndex(0) {
	ClsParser::parseClassFile(classFileData, classFileLen, *this);
}

Version ClassFile::getVersion() const {
	return _version;
}

void ClassFile::setVersion(Version version) {
	_version = version;
}

const char* ClassFile::getThisClassName() const {
	return getClassName(thisClassIndex);
}

Field& ClassFile::addField(ConstPool::Index nameIndex,
		ConstPool::Index descIndex, AccessFlags accessFlags) {
	fields.emplace_back(accessFlags, nameIndex, descIndex);
	return fields.back();
}

Field& ClassFile::addField(const char* fieldName, const char* fieldDesc,
		AccessFlags accessFlags) {
	ConstPool::Index nameIndex = addUtf8(fieldName);
	ConstPool::Index descIndex = addUtf8(fieldDesc);

	return addField(nameIndex, descIndex, accessFlags);
}

Method& ClassFile::addMethod(ConstPool::Index nameIndex,
		ConstPool::Index descIndex, AccessFlags accessFlags) {
	methods.emplace_back(accessFlags, nameIndex, descIndex);
	return methods.back();
}

Method& ClassFile::addMethod(const char* methodName, const char* methodDesc,
		AccessFlags accessFlags) {
	ConstPool::Index nameIndex = addUtf8(methodName);
	ConstPool::Index descIndex = addUtf8(methodDesc);

	return addMethod(nameIndex, descIndex, accessFlags);
}

u4 ClassFile::computeSize() {
	return getClassFileSize(*this);
}

void ClassFile::write(u1* fileImage, int fileImageLen) {
	writeClassFile(*this, fileImage, fileImageLen);
}

ostream& operator<<(ostream& os, ClassFile& cf) {
	printClassFile(cf, os);

	return os;
}

}
