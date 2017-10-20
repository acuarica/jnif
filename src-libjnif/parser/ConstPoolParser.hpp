/**
 *
 */
#ifndef JNIF_PARSER_CONSTPOOLPARSER_HPP
#define JNIF_PARSER_CONSTPOOLPARSER_HPP

#include <jnif.hpp>

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
                            throw JnifException("Error while reading tag: ", tag);
                    }
                }
            }
        };
    }
}

#endif
