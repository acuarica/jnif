/*
 * ConstPool.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: luigi
 */
#include "ConstPool.hpp"
#include "../Error.hpp"

namespace jnif::model {


    u4 ConstPool::size() const {
        return entries.size();
    }

    ConstPool::Iterator ConstPool::iterator() const {
        return Iterator(*this, 1);
    }

    ConstPool::Index ConstPool::addClass(ConstPool::Index classNameIndex) {
        return _addSingle(Class({classNameIndex}));
    }

    ConstPool::Index ConstPool::addClass(const char* className) {
        Index classNameIndex = putUtf8(className);
        return addClass(classNameIndex);
    }

    ConstPool::Index ConstPool::addFieldRef(
        ConstPool::Index classIndex,
        ConstPool::Index nameAndTypeIndex)
    {
        return _addSingle(FIELDREF, MemberRef({classIndex, nameAndTypeIndex}));
    }

    ConstPool::Index ConstPool::addMethodRef(
        ConstPool::Index classIndex,
        ConstPool::Index nameAndTypeIndex)
    {
        return _addSingle(METHODREF, MemberRef({classIndex, nameAndTypeIndex}));
    }

    ConstPool::Index ConstPool::addMethodRef(
        ConstPool::Index classIndex,
        const char* name,
        const char* desc)
    {
        Index methodNameIndex = addUtf8(name);
        Index methodDescIndex = addUtf8(desc);
        Index nameAndTypeIndex = addNameAndType(methodNameIndex, methodDescIndex);
        return addMethodRef(classIndex, nameAndTypeIndex);
    }

    ConstPool::Index ConstPool::addInterMethodRef(
        ConstPool::Index classIndex,
        ConstPool::Index nameAndTypeIndex)
    {
        return _addSingle(INTERMETHODREF,MemberRef({classIndex, nameAndTypeIndex}));
    }

    ConstPool::Index ConstPool::addString(ConstPool::Index utf8Index) {
        return _addSingle(String({utf8Index}));
    }

    ConstPool::Index ConstPool::addString(const std::string& str) {
        Index utf8Index = addUtf8(str.c_str());
        return addString(utf8Index);
    }

    ConstPool::Index ConstPool::addStringFromClass(ConstPool::Index classIndex) {
        Index classNameIndex = getClassNameIndex(classIndex);
        Index classNameStringIndex = addString(classNameIndex);

        return classNameStringIndex;
    }

    ConstPool::Index ConstPool::addInteger(int value) {
        return _addSingle(Integer({value}));
    }

    ConstPool::Index ConstPool::addFloat(float value) {
        return _addSingle(Float({value}));
    }

    ConstPool::Index ConstPool::addLong(long value) {
        return _addDoubleEntry(Long({value}));
    }

    ConstPool::Index ConstPool::addDouble(double value) {
        return _addDoubleEntry(Double({value}));
    }

    ConstPool::Index ConstPool::addNameAndType(ConstPool::Index nameIndex,
                                               ConstPool::Index descIndex) {
        return _addSingle(NameAndType({nameIndex, descIndex}));
    }

    ConstPool::Index ConstPool::addUtf8(const char* utf8, int len) {
        std::string str(utf8, len);
        Index i = _addSingle(str);
        utf8s[str] = i;

        return i;
    }

    ConstPool::Index ConstPool::addUtf8(const char* str) {
        Index i = _addSingle(str);
        utf8s[std::string(str)] = i;

        return i;
    }

    ConstPool::Index ConstPool::addMethodHandle(u1 refKind, u2 refIndex) {
        return _addSingle(MethodHandle({refKind, refIndex}));
    }

    ConstPool::Index ConstPool::addMethodType(u2 descIndex) {
        return _addSingle(MethodType({descIndex}));
    }

    ConstPool::Index ConstPool::addInvokeDynamic(u2 bootstrapMethodAttrIndex,
                                                 u2 nameAndType) {
        return _addSingle(InvokeDynamic({bootstrapMethodAttrIndex, nameAndType}));
    }

    template<class... TArgs>
    ConstPool::Index ConstPool::_addSingle(TArgs... args) {
        int index = entries.size();

        JnifError::check(index < (1<<16), "CP limit reach: index=", index);
        entries.emplace_back(args...);

        return (Index)index;
    }

    template<class... TArgs>
    ConstPool::Index ConstPool::_addDoubleEntry(TArgs... args) {
        Index index = entries.size();
        entries.emplace_back(args...);

        entries.emplace_back();

        return index;
    }

    ConstPool::Index ConstPool::getIndexOfUtf8(const char* utf8) {
        auto it = utf8s.find(utf8);
        if (it != utf8s.end()) {
            Index idx = it->second;
            JnifError::assert(getUtf8(idx) != utf8, "Error on get index of utf8");
            return idx;
        } else {
            return NULLENTRY;
        }
    }

    ConstPool::Index ConstPool::getIndexOfClass(const char* className) {
        auto it = classes.find(className);
        if (it != utf8s.end()) {
            ConstPool::Index idx = it->second;
            // JnifError::assert(getUtf8(idx) != utf8, "Error on get index of utf8");
            return idx;
        } else {
            return NULLENTRY;
        }
    }

    const ConstPool::Item* ConstPool::_getEntry(ConstPool::Index i) const {
        JnifError::check(i > NULLENTRY, "Null access to constant pool: index=", i);
        JnifError::check(i < entries.size(), "Index out of bounds: index=", i);

        const Item* entry = &entries[i];
        return entry;
    }

    const ConstPool::Item* ConstPool::_getEntry(
        ConstPool::Index index, u1 tag, const char* message) const
    {
        const Item* entry = _getEntry(index);
        JnifError::check(entry->tag == tag, "Invalid constant ", message,
                         ", expected: ", (int) tag, ", actual: ", (int) entry->tag);

        return entry;
    }

}
