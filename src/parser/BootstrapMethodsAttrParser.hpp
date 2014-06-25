/*
 * BootstrapMethodsAttrParser.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_BOOTSTRAPMETHODSATTRPARSER_HPP
#define JNIF_PARSER_BOOTSTRAPMETHODSATTRPARSER_HPP

namespace jnif {

// TODO: To implement

class BootstrapMethodsAttrParser {
public:

	BootstrapMethodsAttrParser(ClassFile& cf) :
			_cf(cf) {
	}

	void parse(BufferReader& br) {
		u2 num_bootstrap_methods = br.readu2();

		for (u2 i = 0; i < num_bootstrap_methods; i++) {
			u2 bootstrap_method_ref = br.readu2();
			u2 num_bootstrap_arguments = br.readu2();

			for (u2 arg = 0; i < num_bootstrap_arguments; arg++) {
				u2 bootstrap_argument = br.readu2();
			}
		}
	}

private:

	ClassFile& _cf;
};

}

#endif
