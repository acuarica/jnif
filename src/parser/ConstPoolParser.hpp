/**
 *
 */
#ifndef JNIF_PARSER_CONSTPOOLPARSER_HPP
#define JNIF_PARSER_CONSTPOOLPARSER_HPP

#include "../ConstPool.hpp"

namespace jnif::parser {

    class ConstPoolParser {
    public:

        void parse(BufferReader* br, ConstPool* cp) {
            u2 count = br->readu2();

            for (int i = 1; i < count; i++) {
                u1 tag = br->readu1();

                switch (tag) {
                case CONST_CLASS: {
                    u2 classNameIndex = br->readu2();
                    cp->addClass(classNameIndex);
                    break;
                }
                case CONST_FIELDREF: {
                    u2 classIndex = br->readu2();
                    u2 nameAndTypeIndex = br->readu2();
                    cp->addFieldRef(classIndex, nameAndTypeIndex);
                    break;
                }
                case CONST_METHODREF: {
                    u2 classIndex = br->readu2();
                    u2 nameAndTypeIndex = br->readu2();
                    cp->addMethodRef(classIndex, nameAndTypeIndex);
                    break;
                }
                case CONST_INTERMETHODREF: {
                    u2 classIndex = br->readu2();
                    u2 nameAndTypeIndex = br->readu2();
                    cp->addInterMethodRef(classIndex, nameAndTypeIndex);
                    break;
                }
                case CONST_STRING: {
                    u2 utf8Index = br->readu2();
                    cp->addString(utf8Index);
                    break;
                }
                case CONST_INTEGER: {
                    u4 value = br->readu4();
                    cp->addInteger(value);
                    break;
                }
                case CONST_FLOAT: {
                    u4 value = br->readu4();
                    float fvalue = *(float*) &value;
                    cp->addFloat(fvalue);
                    break;
                }
                case CONST_LONG: {
                    u4 high = br->readu4();
                    u4 low = br->readu4();
                    long value = ((long) high << 32) + low;
                    cp->addLong(value);
                    i++;
                    break;
                }
                case CONST_DOUBLE: {
                    u4 high = br->readu4();
                    u4 low = br->readu4();
                    long lvalue = ((long) high << 32) + low;
                    double dvalue = *(double*) &lvalue;
                    cp->addDouble(dvalue);
                    i++;
                    break;
                }
                case CONST_NAMEANDTYPE: {
                    u2 nameIndex = br->readu2();
                    u2 descIndex = br->readu2();
                    cp->addNameAndType(nameIndex, descIndex);
                    break;
                }
                case CONST_UTF8: {
                    u2 len = br->readu2();
                    cp->addUtf8((const char*) br->pos(), len);
                    br->skip(len);
                    break;
                }
                case CONST_METHODHANDLE: {
                    u1 refKind = br->readu1();
                    u2 refIndex = br->readu2();
                    cp->addMethodHandle(refKind, refIndex);
                    break;
                }
                case CONST_METHODTYPE: {
                    u2 descIndex = br->readu2();
                    cp->addMethodType(descIndex);
                    break;
                }
                case CONST_INVOKEDYNAMIC: {
                    u2 bootMethodAttrIndex = br->readu2();
                    u2 nameAndTypeIndex = br->readu2();
                    cp->addInvokeDynamic(bootMethodAttrIndex, nameAndTypeIndex);
                    break;
                }
                default:
                    JnifError::raise("Error while reading tag: ", tag);
                }
            }
        }
    };
}

#endif
