#include "jnif.hpp"
#include "jniferr.hpp"

namespace jnif {

ConstPool::ConstPool() {
	// TODO: This is plain wrong!!!
	ConstPool::Entry nullEntry(CONSTANT_InvokeDynamic);

	entries.push_back(nullEntry);
}

u4 ConstPool::size() const {
	return entries.size();
}

ConstPool::Index ConstPool::addClass(Index classNameIndex) {
	ConstPool::Entry e(CONSTANT_Class);
	e.clazz.nameIndex = classNameIndex;

	return _addSingle(e);
}

ConstPool::Index ConstPool::addClass(const char* className) {
	Index classNameIndex = addUtf8(className);
	return addClass(classNameIndex);
}

ConstPool::Index ConstPool::addFieldRef(Index classIndex,
		Index nameAndTypeIndex) {
	ConstPool::Entry e(CONSTANT_Fieldref);
	e.memberref.classIndex = classIndex;
	e.memberref.nameAndTypeIndex = nameAndTypeIndex;

	return _addSingle(e);
}

ConstPool::Index ConstPool::addMethodRef(u2 classIndex, u2 nameAndTypeIndex) {
	ConstPool::Entry e(CONSTANT_Methodref);
	e.memberref.classIndex = classIndex;
	e.memberref.nameAndTypeIndex = nameAndTypeIndex;

	return _addSingle(e);
}

ConstPool::Index ConstPool::addMethodRef(Index classIndex, const char* name,
		const char* desc) {
	Index methodNameIndex = addUtf8(name);
	Index methodDescIndex = addUtf8(desc);
	Index nameAndTypeIndex = addNameAndType(methodNameIndex, methodDescIndex);
	Index methodRefIndex = addMethodRef(classIndex, nameAndTypeIndex);

	return methodRefIndex;
}

ConstPool::Index ConstPool::addInterMethodRef(Index classIndex,
		Index nameAndTypeIndex) {
	ConstPool::Entry e(CONSTANT_InterfaceMethodref);
	e.memberref.classIndex = classIndex;
	e.memberref.nameAndTypeIndex = nameAndTypeIndex;

	return _addSingle(e);
}

ConstPool::Index ConstPool::addString(Index utf8Index) {
	ConstPool::Entry e(CONSTANT_String);
	e.s.stringIndex = utf8Index;

	return _addSingle(e);
}

ConstPool::Index ConstPool::addStringFromClass(Index classIndex) {
	Index classNameIndex = getClassNameIndex(classIndex);
	Index classNameStringIndex = addString(classNameIndex);

	return classNameStringIndex;
}

ConstPool::Index ConstPool::addInteger(u4 value) {
	ConstPool::Entry e(CONSTANT_Integer);
	e.i.value = value;

	return _addSingle(e);
}

ConstPool::Index ConstPool::addFloat(u4 value) {
	ConstPool::Entry e(CONSTANT_Float);
	e.f.value = value;

	return _addSingle(e);
}

ConstPool::Index ConstPool::addLong(long value) {
	ConstPool::Entry entry(CONSTANT_Long);
	entry.l.value = value;

	return _addDoubleEntry(entry);
}

ConstPool::Index ConstPool::addDouble(double value) {
	ConstPool::Entry entry(CONSTANT_Double);
	entry.d.value = value;

	return _addDoubleEntry(entry);
}

ConstPool::Index ConstPool::addNameAndType(Index nameIndex, Index descIndex) {
	ConstPool::Entry e(CONSTANT_NameAndType);
	e.nameandtype.nameIndex = nameIndex;
	e.nameandtype.descriptorIndex = descIndex;

	return _addSingle(e);
}

ConstPool::Index ConstPool::addUtf8(const char* utf8, int len) {
	ConstPool::Entry e(CONSTANT_Utf8);
	std::string str(utf8, len);
	e.utf8.str = str;

	return _addSingle(e);
}

ConstPool::Index ConstPool::addUtf8(const char* str) {
	ConstPool::Entry e(CONSTANT_Utf8);
	e.utf8.str = str;

	return _addSingle(e);
}

ConstPool::Index ConstPool::addMethodHandle(u1 refKind, u2 refIndex) {
	ConstPool::Entry e(CONSTANT_MethodHandle);
	e.methodhandle.referenceKind = refKind;
	e.methodhandle.referenceIndex = refIndex;
	return _addSingle(e);
}

ConstPool::Index ConstPool::addMethodType(u2 descIndex) {
	ConstPool::Entry e(CONSTANT_MethodType);
	e.methodtype.descriptorIndex = descIndex;
	return _addSingle(e);

}

ConstPool::Index ConstPool::addInvokeDynamic(u2 bootstrapMethodAttrIndex,
		u2 nameAndTypeIndex) {
	ConstPool::Entry e(CONSTANT_InvokeDynamic);
	e.invokedynamic.bootstrapMethodAttrIndex = bootstrapMethodAttrIndex;
	e.invokedynamic.nameAndTypeIndex = nameAndTypeIndex;
	return _addSingle(e);
}

ConstTag ConstPool::getTag(Index index) const {
	const ConstPool::Entry* entry = _getEntry(index);

	return entry->tag;
}

bool ConstPool::isClass(Index index) const {
	const ConstPool::Entry* entry = _getEntry(index);

	return entry->tag == CONSTANT_Class;
}

ConstPool::Index ConstPool::getClassNameIndex(Index classIndex) const {
	const u2 NULLENTRY = 0;

	const ConstPool::Entry* entry = _getEntry(classIndex, CONSTANT_Class,
			"CONSTANT_Class");

	u2 classNameIndex = entry->clazz.nameIndex;

	ASSERT(classNameIndex != NULLENTRY, "invalid class name index: %d",
			classNameIndex);

	return Index(classNameIndex);
}

void ConstPool::getFieldRef(Index index, string* clazzName, string* name,
		string* desc) const {
	_getMemberRef(index, clazzName, name, desc, CONSTANT_Fieldref);
}

void ConstPool::getMethodRef(Index index, string* clazzName, string* name,
		string* desc) const {
	_getMemberRef(index, clazzName, name, desc, CONSTANT_Methodref);
}

void ConstPool::getInterMethodRef(Index index, string* clazzName, string* name,
		string* desc) const {
	_getMemberRef(index, clazzName, name, desc, CONSTANT_InterfaceMethodref);
}

long ConstPool::getLong(Index index) const {
	return _getEntry(index, CONSTANT_Long, "CONSTANT_Long")->l.value;
}

double ConstPool::getDouble(Index index) const {
	return _getEntry(index, CONSTANT_Double, "CONSTANT_Double")->d.value;
}

const char* ConstPool::getUtf8(Index utf8Index) const {
	const ConstPool::Entry* entry = _getEntry(utf8Index, CONSTANT_Utf8, "Utf8");

	return entry->utf8.str.c_str();
}

const char* ConstPool::getClassName(Index classIndex) const {
	Index classNameIndex = getClassNameIndex(classIndex);

	return getUtf8(classNameIndex);
}

void ConstPool::getNameAndType(Index index, string* name, string* desc) const {
	const ConstPool::Entry* entry = _getEntry(index, CONSTANT_NameAndType,
			"NameAndType");

	u2 nameIndex = entry->nameandtype.nameIndex;
	u2 descIndex = entry->nameandtype.descriptorIndex;

	*name = getUtf8(Index(nameIndex));
	*desc = getUtf8(Index(descIndex));
}

ConstPool::Index ConstPool::_addSingle(const ConstPool::Entry& entry) {
	Index index = Index(entries.size());
	entries.push_back(entry);

	return index;
}

ConstPool::Index ConstPool::_addDoubleEntry(const ConstPool::Entry& entry) {
	Index index = Index(entries.size());

	entries.push_back(entry);

	// TODO: This is plain wrong!!!
	ConstPool::Entry nullEntry(CONSTANT_InvokeDynamic);
	entries.push_back(nullEntry);

	return index;
}

const ConstPool::Entry* ConstPool::_getEntry(Index index) const {
	CHECK(index > NULLENTRY, "Null access to constant pool: index = %d", index);
	CHECK(index < entries.size(), "Index out of bounds: index = %d", index);

	const ConstPool::Entry* entry = &entries[index];

	return entry;
}

const ConstPool::Entry* ConstPool::_getEntry(Index index, u1 tag,
		const char* message) const {
	const ConstPool::Entry* entry = _getEntry(index);

	CHECK(entry->tag == tag, "Invalid constant %s tag", message);

	return entry;
}

bool ConstPool::_isDoubleEntry(Index index) const {
	const ConstPool::Entry* e = _getEntry(index);

	switch (e->tag) {
		case CONSTANT_Long:
		case CONSTANT_Double:
			return true;
		default:
			return false;
	}
}

void ConstPool::_getMemberRef(Index index, std::string* clazzName,
		std::string* name, std::string* desc, u1 tag) const {
	const ConstPool::Entry* entry = _getEntry(index, tag, "memberref");

	Index classIndex = Index(entry->memberref.classIndex);
	Index nameAndTypeIndex = Index(entry->memberref.nameAndTypeIndex);

	*clazzName = getClassName(classIndex);
	getNameAndType(nameAndTypeIndex, name, desc);
}

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

void parseClassFile(const u1* fileImage, const int fileImageLen, ClassFile& cf);
u4 getClassFileSize(ClassFile& cf);
void writeClassFile(ClassFile& cf, u1* fileImage, const int fileImageLen);
void printClassFile(ClassFile& cf, ostream& os, int tabs = 0);

ClassFile::ClassFile(const char* className, const char* superClassName,
		u2 accessFlags, Version version) :
		accessFlags(accessFlags), _version(version), thisClassIndex(
				addClass(className)), superClassIndex(addClass(superClassName)) {
}

ClassFile::ClassFile(const u1* classFileData, const int classFileLen) :
		thisClassIndex(0), superClassIndex(0) {
	parseClassFile(classFileData, classFileLen, *this);
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
