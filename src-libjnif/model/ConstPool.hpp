/*
 * ConstPool.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_MODEL_CONSTPOOL_HPP
#define JNIF_MODEL_CONSTPOOL_HPP

#include "../base.hpp"
#include <vector>
#include <map>

namespace jnif::model {

    using std::map;

    /// Represents the Java class file's constant pool.
    /// Provides the base services to manage the constant pool.
    /// The constant pool is a table which holds
    /// different kinds of items depending on their use. It can
    /// hold references for classes, fields, methods, interface methods, strings,
    /// integers, floats, longs, doubles, name and type, utf8 arrays,
    /// method handles, method types and invoke dynamic bootstrap methods.
    ///
    /// This class works by adding these kinds of entries in an array-like
    /// structure. When an entry is removed, all entries after the one removed will
    /// be invalidated, for this reason, no removed operations are allowed.
    class ConstPool {

        ConstPool(const ConstPool&) = delete;
        ConstPool(ConstPool&&) = delete;
        ConstPool& operator=(const ConstPool&) = delete;
        ConstPool& operator=(ConstPool&&) = delete;

    public:

        /// The Index type represents how each item within the constant pool
        /// can be addressed.
        /// The specification indicates that this is an u2 value.
        typedef u2 Index;

        /// Represents a class, interface or array type.
        /// @see CLASS
        struct Class {

            /// The name of the class of this type.
            /// @see UTF8
            /// @see Utf8
            const Index nameIndex;
        };

        /// Represents a symbolic reference to a member
        /// (field, method or interface method) of a class.
        /// @see FieldRef
        /// @see MethodRef
        /// @see InterMethodRef
        struct MemberRef {

            /// Index to the class of this member reference.
            /// @see Class
            const Index classIndex;

            /// Index to the name and type of this member reference.
            /// @see NameAndType
            const Index nameAndTypeIndex;
        };

        /// Represents a string constant value.
        /// @see STRING
        struct String {

            /// The index of the utf8 entry containing this string value.
            /// @see Utf8
            const Index stringIndex;
        };

        /// Represents an integer constant value.
        /// @see INTEGER
        struct Integer {

            /// The integer value of this entry.
            const int value;
        };

        /// Represent a float constant value.
        /// @see FLOAT
        struct Float {

            /// The float value of this entry.
            const float value;
        };

        /// Represents a long constant value.
        /// @see LONG
        struct Long {

            /// The long value of this entry.
            const long value;
        };

        /// Represents a double constant value.
        /// @see DOUBLE
        struct Double {

            /// The double value of this entry.
            const double value;
        };

        /// Represents a tuple of name and descriptor.
        struct NameAndType {

            /// The index of the utf8 entry containing the name of this entry.
            const Index nameIndex;

            /// The utf8 entry index containing the descriptor of this entry.
            const Index descriptorIndex;
        };

        /// Contains a modified UTF-8 string.
        /// @see UTF8
        struct Utf8 {

            Utf8() {}

            Utf8(const std::string& str) : str(str) {}

            /// The string data.
            const std::string str;
        };

        /// Represents a method handle entry.
        /// @see METHODHANDLE
        struct MethodHandle {

            /// The reference kind of this entry.
            const u1 referenceKind;

            /// The reference index of this entry.
            const u2 referenceIndex;
        };

        /// Represents the type of a method.
        /// @see METHODTYPE
        struct MethodType {

            /// The utf8 index containing the descriptor of this entry.
            const Index descriptorIndex;
        };

        /// Represents an invoke dynamic call site.
        /// @see INVOKEDYNAMIC
        struct InvokeDynamic {

            /// The bootstrap method attribute index.
            const Index bootstrapMethodAttrIndex;

            /// The name and type index of this entry.
            /// @see ConstNameAndType
            const Index nameAndTypeIndex;
        };

        /// Constant pool enum used to distinguish between different kinds of
        /// elements inside the constant pool.
        enum Tag {

            /// Represents the null entry which cannot be addressed.
            /// This is used for the NULLENTRY (position zero) and
            /// for long and double entries.
            NULLENTRY,

            /// Represents a class or an interface.
            CLASS = 7,

            /// Represents a field.
            FIELDREF = 9,

            /// Represents a method.
            METHODREF = 10,

            /// Represents an inteface method.
            INTERMETHODREF = 11,

            /// Used to represent constant objects of the type String.
            STRING = 8,

            /// Represents 4-byte numeric int constants.
            INTEGER = 3,

            /// Represents 4-byte numeric float constants.
            FLOAT = 4,

            /// Represents 8-byte numeric long constants.
            LONG = 5,

            /// Represents 8-byte numeric double constants.
            DOUBLE = 6,

            /// Used to represent a field or method, without indicating which class
            /// or interface type it belongs to.
            NAMEANDTYPE = 12,

            /// Used to represent constant string values.
            UTF8 = 1,

            /// Used to represent a method handle.
            METHODHANDLE = 15,

            /// Used to represent a method type.
            METHODTYPE = 16,

            /// Used by an invokedynamic instruction to specify a bootstrap method,
            /// the dynamic invocation name, the argument and return types of the
            /// call, and optionally, a sequence of additional constants called
            /// static arguments to the bootstrap method.
            INVOKEDYNAMIC = 18
        };

        /// The const item.
        struct Item {

            Item() : tag(NULLENTRY) {}
            explicit Item(Class clazz) : tag(CLASS), clazz(clazz) {}
            Item(Tag tag, MemberRef memberRef) : tag(tag), memberRef(memberRef) {}
            Item(String s) : tag(STRING), s(s) {}
            Item(Integer i) : tag(INTEGER), i(i) {}
            Item(Float f) : tag(FLOAT), f(f) {}
            Item(Long l) : tag(LONG), l(l) {}
            Item(Double d) : tag(DOUBLE), d(d) {}
            Item(NameAndType nat) : tag(NAMEANDTYPE), nameandtype(nat) {}
            Item(MethodHandle mh) : tag(METHODHANDLE), methodhandle(mh) {}
            Item(MethodType mt) : tag(METHODTYPE), methodtype(mt) {}
            Item(InvokeDynamic id) : tag(INVOKEDYNAMIC), invokedynamic(id) {}
            Item(const std::string& value) : tag(UTF8), utf8(value) {}

            Item(const Item&) = delete;
            Item& operator=(const Item&) = delete;
            Item(Item&&) = default;

            const Tag tag;

            union {
                Class clazz;
                MemberRef memberRef;
                String s;
                Integer i;
                Float f;
                Long l;
                Double d;
                NameAndType nameandtype;
                MethodHandle methodhandle;
                MethodType methodtype;
                InvokeDynamic invokedynamic;
            };

            const Utf8 utf8;

        };

        /**
         * Defines how to iterate the constant pool.
         */
        class Iterator {
            friend class ConstPool;
        public:

            bool hasNext() const {
                return index < cp.size();
            }

            Index operator*() const {
                return index;
            }

            Iterator& operator++(int) {
                index += cp._isDoubleEntry(index) ? 2 : 1;

                return *this;
            }

        private:
            Iterator(const ConstPool& cp, Index index) :
                cp(cp), index(index) {
            }

            const ConstPool& cp;
            Index index;
        };

        /// Represents the invalid (null) item, which must not be asked for.
        static const Index NULLINDEX = 0;

        /// Initializes an empty constant pool. The valid indices start from 1
        /// inclusive, because the null entry (index 0) is added by default.
        ConstPool(size_t initialCapacity = 4096) {
            entries.reserve(initialCapacity);
            entries.emplace_back();
        }

        /// Returns the number of elements in this constant pool.
        /// @returns number of elements in this constant pool.
        u4 size() const;

        /**
         *
         */
        Iterator iterator() const;

        /// Adds a class reference to the constant pool.
        /// @param classNameIndex the utf8 index that represents the name of this
        /// class item.
        /// @returns the index of the newly created reference to a class item.
        Index addClass(Index classNameIndex);

        /**
         * Adds a class reference to the constant pool by class name. This method
         * adds also the utf8 entry corresponding the class name itself.
         *
         * @param className name of the class to reference.
         * @returns the index of the newly created reference to a class item.
         */
        Index addClass(const char* className);

        /**
         * Adds a field reference to this constant pool.
         *
         * @param classIndex the symbolic class that this field belongs to.
         * @param nameAndTypeIndex the name and type symbolic reference
         * describing the name and type of the field to add.
         * @returns the index of the newly created entry.
         */
        Index addFieldRef(Index classIndex, Index nameAndTypeIndex);

        /**
         * Adds a class method reference.
         *
         * @returns the Index of the newly created entry.
         */
        Index addMethodRef(Index classIndex, Index nameAndTypeIndex);

        /**
         * Adds a non-interface methods by method name and descriptor.
         *
         * @returns the Index of the newly created entry.
         */
        Index addMethodRef(Index classIndex, const char* name, const char* desc);

        /// Adds an interface method reference.
        /// @returns the Index of the newly created entry.
        Index addInterMethodRef(Index classIndex, Index nameAndTypeIndex);

        /**
         * @returns the Index of the newly created entry.
         */
        Index addString(Index utf8Index);

        /**
         * Adds a string to the constant pool by providing a string.
         */
        Index addString(const std::string& str);

        /**
         * Adds a String item to the Constant Pool using a class entry.
         *
         * @returns the Index of the newly created String entry.
         */
        Index addStringFromClass(Index classIndex);

        /**
         * Adds an integer constant value.
         *
         * @param value the integer value.
         * @returns the Index of the newly created entry.
         */
        Index addInteger(int value);

        /**
         * Adds a float constant value.
         *
         * @param value the float value.
         * @returns the Index of the newly created entry.
         */
        Index addFloat(float value);

        /**
         * Adds a long constant value.
         *
         * @param value the long value.
         * @returns the Index of the newly created entry.
         */
        Index addLong(long value);

        /**
         * Adds a double constant value.
         *
         * @param value the double value.
         * @returns the Index of the newly created entry.
         */
        Index addDouble(double value);

        /**
         * Adds a name and type descriptor pair.
         *
         * @param nameIndex the index of the UTF8 entry with the name.
         * @param descIndex the index of the UTF8 entry with the type descriptor.
         * @returns the Index of the newly created entry.
         */
        Index addNameAndType(Index nameIndex, Index descIndex);

        /**
         * Adds a modified UTF8 string given the char array and its len.
         *
         * @param utf8 the char array containing the modified UTF8 string.
         * @param len the len in bytes of utf8.
         * @returns the Index of the newly created entry.
         */
        Index addUtf8(const char* utf8, int len);

        /**
         * Adds a modified UTF8 string given an null-terminated char array.
         *
         * @param str the null-terminated char array containing the modified
         * UTF8 string.
         * @returns the Index of the newly created entry.
         */
        Index addUtf8(const char* str);

        /**
         * @returns the Index of the newly created entry.
         */
        Index addMethodHandle(u1 refKind, u2 refIndex);

        /// @returns the Index of the newly created entry.
        Index addMethodType(u2 descIndex);

        /// @returns the Index of the newly created entry.
        Index addInvokeDynamic(u2 bootstrapMethodAttrIndex, u2 nameAndTypeIndex);

        /**
         *
         */
        Tag getTag(Index index) const {
            const Item* entry = _getEntry(index);
            return entry->tag;
        }

        /**
         * Checks whether the requested index holds a class reference.
         */
        bool isClass(Index index) const {
            return _getEntry(index)->tag == CLASS;
        }

        bool isUtf8(Index index) const {
            return _getEntry(index)->tag == UTF8;
        }

        Index getClassNameIndex(Index classIndex) const {
            const Item* e = _getEntry(classIndex, CLASS, "CONSTANT_Class");
            Index ni = e->clazz.nameIndex;

            return ni;
        }

        void getFieldRef(Index index, std::string* className, std::string* name,
                         std::string* desc) const {
            const Item* e = _getEntry(index, FIELDREF, "FieldRef");
            const MemberRef& mr = e->memberRef;
            _getMemberRef(className, name, desc, mr);
        }

        void getMethodRef(Index index, std::string* clazzName, std::string* name,
                          std::string* desc) const {
            const Item* e = _getEntry(index, METHODREF, "MethodRef");
            const MemberRef& mr = e->memberRef;
            _getMemberRef(clazzName, name, desc, mr);
        }

        void getInterMethodRef(
            Index index, std::string* clazzName, std::string* name,
            std::string* desc) const
        {
            const Item* e = _getEntry(index, INTERMETHODREF, "imr");
            const MemberRef& mr = e->memberRef;
            _getMemberRef(clazzName, name, desc, mr);
        }

        const char* getString(Index index) const {
            const Item* e = _getEntry(index, STRING, "String");
            return getUtf8(e->s.stringIndex);
        }

        int getInteger(Index index) const {
            return _getEntry(index, INTEGER, "CONSTANT_Integer")->i.value;
        }

        float getFloat(Index index) const {
            return _getEntry(index, FLOAT, "CONSTANT_Float")->f.value;
        }

        long getLong(Index index) const {
            return _getEntry(index, LONG, "CONSTANT_Long")->l.value;
        }

        double getDouble(Index index) const {
            return _getEntry(index, DOUBLE, "CONSTANT_Double")->d.value;
        }

        const char* getUtf8(Index utf8Index) const {
            const Item* entry = _getEntry(utf8Index, UTF8, "Utf8");
            return entry->utf8.str.c_str();
        }

        const char* getClassName(Index classIndex) const {
            Index classNameIndex = getClassNameIndex(classIndex);
            return getUtf8(classNameIndex);
        }

        void getNameAndType(Index index, std::string* name, std::string* desc) const {
            const Item* e = _getEntry(index, NAMEANDTYPE, "NameAndType");
            u2 nameIndex = e->nameandtype.nameIndex;
            u2 descIndex = e->nameandtype.descriptorIndex;

            *name = getUtf8(nameIndex);
            *desc = getUtf8(descIndex);
        }

        const InvokeDynamic& getInvokeDynamic(Index index) const {
            const Item* e = _getEntry(index, INVOKEDYNAMIC, "Indy");
            return e->invokedynamic;
        }

        Index getIndexOfUtf8(const char* utf8);
        Index getIndexOfClass(const char* className);

        Index putUtf8(const char* utf8) {
            Index i = getIndexOfUtf8(utf8);
            if (i == NULLENTRY) {
                return addUtf8(utf8);
            } else {
                return i;
            }
        }

        Index putClass(const char* className) {
            Index i = getIndexOfClass(className);
            if (i == NULLENTRY) {
                i = addClass(className);
                classes[className] = i;
                return i;
            } else {
                return i;
            }
        }

        std::vector<Item> entries;

    private:

        template<class... TArgs>
        Index _addSingle(TArgs... args);

        template<class... TArgs>
        Index _addDoubleEntry(TArgs... args);

        const Item* _getEntry(Index i) const;

        const Item* _getEntry(Index index, u1 tag, const char* message) const;

        bool _isDoubleEntry(Index index) const {
            const Item* e = _getEntry(index);
            return e->tag == LONG || e->tag == DOUBLE;
        }

        void _getMemberRef(
            std::string* clazzName, std::string* name, std::string* desc,
            const MemberRef& memberRef) const {
            Index classIndex = memberRef.classIndex;
            Index nameAndTypeIndex = memberRef.nameAndTypeIndex;

            *clazzName = getClassName(classIndex);
            getNameAndType(nameAndTypeIndex, name, desc);
        }

        std::map<std::string, Index> utf8s;
        std::map<std::string, Index> classes;
    };

    std::ostream& operator<<(std::ostream& os, const ConstPool::Tag& tag);

}

#endif
