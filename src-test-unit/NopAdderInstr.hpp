/*
 * NopAdderInstr.hpp
 *
 *  Created on: Mar 4, 2014
 *      Author: luigi
 */

#ifndef NOPADDERINSTR_HPP_
#define NOPADDERINSTR_HPP_

#include "jnif.hpp"

struct NopAdderInstr {

	template<typename TVisitor>
	struct Method: ClassParser::MethodForward<TVisitor> {
		using ClassParser::MethodForward<TVisitor>::mv;

		Method(TVisitor& mv) :
				ClassParser::MethodForward<TVisitor>(mv) {
		}

		inline void codeStart() {
			mv.visitZero(-42, OPCODE_nop);
			mv.visitZero(-42, OPCODE_nop);
		}
	};

	template<typename TVisitor>
	struct Class: ClassParser::ClassForward<TVisitor> {
		typedef ClassParser::ClassForward<TVisitor> base;
		using base::cv;

		Class(TVisitor& cv) :
				ClassParser::ClassForward<TVisitor>(cv) {
		}

		auto visitMethod(u2 accessFlags, u2 nameIndex,
				u2 descIndex)-> Method<decltype(base::cv.visitMethod(0, 0, 0))> {
			auto mv = base::cv.visitMethod(accessFlags, nameIndex, descIndex);

			return Method<decltype(mv)>(mv);
		}
	};
};

#endif
