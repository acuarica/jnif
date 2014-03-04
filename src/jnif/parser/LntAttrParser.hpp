#ifndef JNIF_LNTATTRPARSER_HPP
#define JNIF_LNTATTRPARSER_HPP

#include "../base.hpp"
#include "../tree/ConstPool.hpp"

namespace jnif {

/**
 *
 * Line number table
 */
struct LntAttrParser {

	static constexpr const char* AttrName = "LineNumberTable";

	template<typename TReader, typename TVisitor>
	inline void parse(TReader& br, TVisitor& v, ConstPool& cp, u2 nameIndex) {
		u2 lntlen = br.readu2();

		v.visitLntCount(lntlen);

		for (int i = 0; i < lntlen; i++) {
			u2 startpc = br.readu2();
			u2 lineno = br.readu2();

			v.visitLntEntry(nameIndex, startpc, lineno);
		}
	}

	template<typename TWriter>
	struct Writer {
		TWriter& w;

		inline Writer(TWriter& w) :
				w(w) {
		}

		inline void visitLntCount(u2 lntlen) {
			w.writeu2(lntlen);
		}

		inline void visitLntEntry(u2 nameIndex, u2 startpc, u2 lineno) {
			w.writeu2(startpc);
			w.writeu2(lineno);
		}
	};

	template<typename TVisitor>
	struct Forward {
		TVisitor av;

		inline Forward(TVisitor& av) :
				av(av) {
		}

		inline void visitLntCount(u2 lntlen) {
			av.visitLntCount(lntlen);
		}

		inline void visitLntEntry(u2 nameIndex, u2 startpc, u2 lineno) {
			av.visitLntEntry(nameIndex, startpc, lineno);
		}
	};
};

}

#endif
