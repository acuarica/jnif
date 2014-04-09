/*
 * DescParser.hpp
 *
 *  Created on: Apr 7, 2014
 *      Author: luigi
 */

#ifndef JNIF_DESCPARSER_HPP
#define JNIF_DESCPARSER_HPP

namespace jnif {

class DescParser: protected ErrorManager {
public:

	static Type parseFieldDesc(const char*& fieldDesc) {
		const char* originalFieldDesc = fieldDesc;

		int dims = 0;
		while (*fieldDesc == '[') {
			check(*fieldDesc != '\0',
					"Reach end of string while searching for array. Field descriptor: ",
					originalFieldDesc);
			fieldDesc++;
			dims++;
		}

		check(*fieldDesc != '\0', "");

		auto parseBaseType = [&] () {
			switch (*fieldDesc) {
				case 'Z':
				case 'B':
				case 'C':
				case 'S':
				case 'I':
				return Type::intt();
				case 'D':
				return Type::doublet();
				case 'F':
				return Type::floatt();
				case 'J':
				return Type::longt();
				case 'L': {
					fieldDesc++;

					//const char* className = fieldDesc;
				int len = 0;
				while (*fieldDesc != ';') {
					check(*fieldDesc != '\0', "");
					fieldDesc++;
					len++;
				}
				return Type::objectt(-1);
			}
			default:
			raise("Invalid field desc ", originalFieldDesc);
		}};

		Type t = [&]() {
			Type baseType = parseBaseType();
			if (dims == 0) {
				return baseType;
			} else {
				return Type::objectt(-2);
			}
		}();

		fieldDesc++;

		return t;
	}

	static Type parseMethodDesc(const char* methodDesc,
			std::vector<Type>* argsType) {
		const char* originalMethodDesc = methodDesc;

		check(*methodDesc == '(', "Invalid beginning of method descriptor: ",
				originalMethodDesc);
		methodDesc++;

		while (*methodDesc != ')') {
			check(*methodDesc != '\0', "Reached end of string: ",
					originalMethodDesc);

			Type t = parseFieldDesc(methodDesc);
			argsType->push_back(t);
		}

		check(*methodDesc == ')', "Expected ')' in method descriptor: ",
				originalMethodDesc);
		methodDesc++;

		check(*methodDesc != '\0', "Reached end of string: ",
				originalMethodDesc);

		Type returnType = [&]() {
			if (*methodDesc == 'V') {
				methodDesc++;
				return Type::voidType();
			} else {
				return parseFieldDesc(methodDesc);
			}
		}();

		check(*methodDesc == '\0', "Expected end of string: %s",
				originalMethodDesc);

		return returnType;
	}
};

}

#endif
