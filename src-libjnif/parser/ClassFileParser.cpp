
#include <jnif.hpp>

#include "BufferReader.hpp"
//#include "ConstPoolParser.hpp"
//#include "ClassParser.hpp"
#include "CodeAttrParser.hpp"
#include "ExceptionsAttrParser.hpp"
#include "LineNumberTableAttrParser.hpp"
#include "LocalVariableTableAttrParser.hpp"
#include "LocalVariableTypeTableAttrParser.hpp"
#include "StackMapTableAttrParser.hpp"
#include "SourceFileAttrParser.hpp"
#include "SignatureAttrParser.hpp"

namespace jnif {

    namespace parser {

        struct ConstPoolParser {

            void parse(BufferReader* br, ConstPool* cp) {
                u2 count = br->readu2();

                for (int i = 1; i < count; i++) {
                    u1 tag = br->readu1();

                    switch (tag) {
                        case ConstPool::CLASS: {
                            u2 classNameIndex = br->readu2();
                            cp->addClass(classNameIndex);
                            break;
                        }
                        case ConstPool::FIELDREF: {
                            u2 classIndex = br->readu2();
                            u2 nameAndTypeIndex = br->readu2();
                            cp->addFieldRef(classIndex, nameAndTypeIndex);
                            break;
                        }
                        case ConstPool::METHODREF: {
                            u2 classIndex = br->readu2();
                            u2 nameAndTypeIndex = br->readu2();
                            cp->addMethodRef(classIndex, nameAndTypeIndex);
                            break;
                        }
                        case ConstPool::INTERMETHODREF: {
                            u2 classIndex = br->readu2();
                            u2 nameAndTypeIndex = br->readu2();
                            cp->addInterMethodRef(classIndex, nameAndTypeIndex);
                            break;
                        }
                        case ConstPool::STRING: {
                            u2 utf8Index = br->readu2();
                            cp->addString(utf8Index);
                            break;
                        }
                        case ConstPool::INTEGER: {
                            u4 value = br->readu4();
                            cp->addInteger(value);
                            break;
                        }
                        case ConstPool::FLOAT: {
                            u4 value = br->readu4();
                            float fvalue = *(float*) &value;
                            cp->addFloat(fvalue);
                            break;
                        }
                        case ConstPool::LONG: {
                            u4 high = br->readu4();
                            u4 low = br->readu4();
                            long value = ((long) high << 32) + low;
                            cp->addLong(value);
                            i++;
                            break;
                        }
                        case ConstPool::DOUBLE: {
                            u4 high = br->readu4();
                            u4 low = br->readu4();
                            long lvalue = ((long) high << 32) + low;
                            double dvalue = *(double*) &lvalue;
                            cp->addDouble(dvalue);
                            i++;
                            break;
                        }
                        case ConstPool::NAMEANDTYPE: {
                            u2 nameIndex = br->readu2();
                            u2 descIndex = br->readu2();
                            cp->addNameAndType(nameIndex, descIndex);
                            break;
                        }
                        case ConstPool::UTF8: {
                            u2 len = br->readu2();
                            cp->addUtf8((const char*) br->pos(), len);
                            br->skip(len);
                            break;
                        }
                        case ConstPool::METHODHANDLE: {
                            u1 refKind = br->readu1();
                            u2 refIndex = br->readu2();
                            cp->addMethodHandle(refKind, refIndex);
                            break;
                        }
                        case ConstPool::METHODTYPE: {
                            u2 descIndex = br->readu2();
                            cp->addMethodType(descIndex);
                            break;
                        }
                        case ConstPool::INVOKEDYNAMIC: {
                            u2 bootMethodAttrIndex = br->readu2();
                            u2 nameAndTypeIndex = br->readu2();
                            cp->addInvokeDynamic(bootMethodAttrIndex, nameAndTypeIndex);
                            break;
                        }
                        default:
                            throw Exception("Error while reading tag: ", tag);
                    }
                }
            }
        };

        template<class... TAttrParsers>
        struct AttrParser {
            template<class... TArgs>
            Attr* parse(
                    u2 nameIndex, u4 len, const u1* data, const string&,
                    ClassFile* cp, TArgs...
            ) {
                return cp->_arena.create<UnknownAttr>(nameIndex, len, data, cp);
            }
        };

        template<class TAttrParser, class... TAttrParsers>
        struct AttrParser<TAttrParser, TAttrParsers...> : AttrParser<TAttrParsers...> {

            template<class... TArgs>
            Attr* parse(u2 nameIndex, u4 len, const u1* data, const string& attrName,
                        ClassFile* cp, TArgs... args) {
                if (attrName == TAttrParser::AttrName) {
                    BufferReader br(data, len);
                    return TAttrParser().parse(&br, cp, nameIndex, args...);
                } else {
                    return AttrParser<TAttrParsers...>::parse(
                            nameIndex, len, data, attrName, cp, args...);
                }
            }

        };

        template<class... TAttrParsers>
        struct AttrsParser {

            template<class... TArgs>
            void parse(BufferReader* br, ClassFile* cp, Attrs* as, TArgs... args) {
                u2 attrCount = br->readu2();

                for (int i = 0; i < attrCount; i++) {
                    u2 nameIndex = br->readu2();
                    u4 len = br->readu4();
                    const u1* data = br->pos();

                    br->skip(len);

                    string attrName = cp->getUtf8(nameIndex);

                    Attr* a = AttrParser<TAttrParsers...>().parse(
                            nameIndex, len, data, attrName, cp, args...);
                    as->add(a);
                }
            }

        };

        /**
         * Represents an abstract java class file parser.
         *
         * Only suitable when TClassAttrsParser, TMethodAttrsParser and
         * TFieldAttrsParser are AttrsParser classes.
         */
        template<
                typename TConstPoolParser,
                typename TClassAttrsParser,
                typename TMethodAttrsParser,
                typename TFieldAttrsParser>
        class ClassParser;

        /**
         * Represents an abstract java class file parser.
         *
         * Instantiation of the parser implementation.
         *
         * The template instantiation is the only one accepted, since it receives
         * the members and class attributes parsers as templates lists.
         */
        template<
                typename TConstPoolParser,
                typename ... TClassAttrParserList,
                typename ... TMethodAttrParserList,
                typename ... TFieldAttrParserList>
        class ClassParser<
                TConstPoolParser,
                AttrsParser<TClassAttrParserList...>,
                AttrsParser<TMethodAttrParserList...>,
                AttrsParser<TFieldAttrParserList...>
        > {
        public:

            typedef AttrsParser<TClassAttrParserList...> ClassAttrsParser;

            typedef AttrsParser<TMethodAttrParserList...> MethodAttrsParser;

            typedef AttrsParser<TFieldAttrParserList...> FieldAttrsParser;

            /**
             *
             */
            void parse(BufferReader* br, ClassFile* cf) {
                u4 magic = br->readu4();

                JnifError::check(
                        magic == ClassFile::MAGIC,
                        "Invalid magic number. Expected 0xcafebabe, found: ",
                        magic);

                u2 minorVersion = br->readu2();
                u2 majorVersion = br->readu2();

                cf->version = Version(majorVersion, minorVersion);

                TConstPoolParser().parse(br, cf);

                cf->accessFlags = br->readu2();
                cf->thisClassIndex = br->readu2();
                cf->superClassIndex = br->readu2();

                u2 interCount = br->readu2();
                for (int i = 0; i < interCount; i++) {
                    u2 interIndex = br->readu2();
                    cf->interfaces.push_back(interIndex);
                }

                u2 fieldCount = br->readu2();
                for (int i = 0; i < fieldCount; i++) {
                    u2 accessFlags = br->readu2();
                    u2 nameIndex = br->readu2();
                    u2 descIndex = br->readu2();

                    Field& f = cf->addField(nameIndex, descIndex, accessFlags);
                    FieldAttrsParser().parse(br, cf, &f.attrs);
                }

                u2 methodCount = br->readu2();
                for (int i = 0; i < methodCount; i++) {
                    u2 accessFlags = br->readu2();
                    u2 nameIndex = br->readu2();
                    u2 descIndex = br->readu2();

                    Method& m = cf->addMethod(nameIndex, descIndex, accessFlags);
                    MethodAttrsParser().parse(br, cf, &m.attrs);
                }

                ClassAttrsParser().parse(br, cf, &cf->attrs);
            }

        };

        ClassFileParser::ClassFileParser(const u1* data, u4 len) {
            parse(data, len, this);
        }

        void ClassFileParser::parse(const u1* data, u4 len, ClassFile* classFile) {
            BufferReader br(data, len);
            ClassParser<
                    ConstPoolParser,
                    AttrsParser<
                            SourceFileAttrParser,
                            SignatureAttrParser>,
                    AttrsParser<
                            CodeAttrParser<
                                    LineNumberTableAttrParser,
                                    LocalVariableTableAttrParser,
                                    LocalVariableTypeTableAttrParser,
                                    StackMapTableAttrParser>,
                            ExceptionsAttrParser,
                            SignatureAttrParser>,
                    AttrsParser<
                            SignatureAttrParser>
            > parser;
            parser.parse(&br, classFile);
        }

    }
}
