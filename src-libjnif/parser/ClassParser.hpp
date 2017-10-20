/*
 * ClassParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_CLASSPARSER_HPP
#define JNIF_PARSER_CLASSPARSER_HPP

#include <jnif.hpp>
#include "AttrsParser.hpp"

namespace jnif { namespace parser {

    using namespace model;

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

}}

#endif