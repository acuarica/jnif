/*
 * DescParser.hpp
 *
 *  Created on: Apr 7, 2014
 *      Author: luigi
 */

#ifndef JNIF_DESCPARSER_HPP
#define JNIF_DESCPARSER_HPP

namespace jnif {

class DescParser: private ErrorManager {
public:

	template<typename THandler>
	static void parseFieldDesc(const char*& fieldDesc, THandler& h) {
		const char* originalFieldDesc = fieldDesc;

		int ds = 0;
		while (*fieldDesc == '[') {
			check(*fieldDesc != '\0',
					"Reach end of string while searching for array. Field descriptor: ",
					originalFieldDesc);
			fieldDesc++;
			ds++;
		}

		if (ds > 0) {
			h.arrayType(ds);
		}

		check(*fieldDesc != '\0', "");

		switch (*fieldDesc) {
			case 'Z':
			case 'B':
			case 'C':
			case 'S':
			case 'I':
				h.intType();
				break;
			case 'D':
				h.doubleType();
				break;
			case 'F':
				h.floatType();
				break;
			case 'J':
				h.longType();
				break;
			case 'L': {
				fieldDesc++;

				//const char* className = fieldDesc;
				int len = 0;
				while (*fieldDesc != ';') {
					check(*fieldDesc != '\0', "");
					fieldDesc++;
					len++;
				}
				h.refType();

				break;
			}
			default:
				raise("Invalid field desc ", originalFieldDesc);
		}

		fieldDesc++;
	}

	template<typename THandler>
	static void parseMethodDesc(const char* methodDesc, THandler& h,
			int lvstart) {
		const char* originalMethodDesc = methodDesc;

		check(*methodDesc == '(', "Invalid beginning of method descriptor: ",
				originalMethodDesc);
		methodDesc++;

		int lvindex = lvstart;
		while (*methodDesc != ')') {
			check(*methodDesc != '\0', "Reached end of string: ",
					originalMethodDesc);

			struct ParseMethodAdapter {
				THandler& h;
				int lvindex;
				int dims;
				ParseMethodAdapter(THandler& h, int lvindex) :
						h(h), lvindex(lvindex), dims(0) {
				}
				void intType() {
					if (this->dims == 0) {
						this->h.setIntVar(this->lvindex);
					}
				}
				void longType() {
					if (this->dims == 0) {
						this->h.setLongVar(this->lvindex);
					}
				}
				void floatType() {
					if (this->dims == 0) {
						this->h.setFloatVar(this->lvindex);
					}
				}
				void doubleType() {
					if (this->dims == 0) {
						this->h.setDoubleVar(this->lvindex);
					}
				}
				void refType() {
					if (this->dims == 0) {
						this->h.setRefVar(this->lvindex);
					}
				}
				void arrayType(int ds) {
					this->dims = ds;
					this->h.setRefVar(this->lvindex);
				}
			} a(h, lvindex);

			parseFieldDesc(methodDesc, a);
			lvindex++;
		}

		check(*methodDesc == ')', "Expected ')' in method descriptor: ",
				originalMethodDesc);
		methodDesc++;

		check(*methodDesc != '\0', "Reached end of string: ",
				originalMethodDesc);

		if (*methodDesc == 'V') {
			methodDesc++;
			//h.typeVoid();
		} else {
			struct {
				void intType() {
				}
				void longType() {
				}
				void floatType() {
				}
				void doubleType() {
				}
				void refType() {
				}
				void arrayType(int) {
				}
			} e;
			parseFieldDesc(methodDesc, e);
		}

		check(*methodDesc == '\0', "Expected end of string: %s",
				originalMethodDesc);
	}
};

}

#endif
