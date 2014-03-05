#ifndef JNIF_EXCEPTIONSATTRPARSER_HPP
#define JNIF_EXCEPTIONSATTRPARSER_HPP

#include "../base.hpp"
#include "../tree/ConstPool.hpp"

namespace jnif {

/**
 * Exceptions
 */
struct ExceptionsAttrParser {

	static constexpr const char* AttrName = "Exceptions";

	template<typename TReader, typename TVisitor>
	static void parse(TReader& br, TVisitor& v, ConstPool& cp, u2 nameIndex) {
		u2 len = br.readu2();

		v.visitExceptionCount(len);

		for (int i = 0; i < len; i++) {
			u2 exceptionIndex = br.readu2();

			v.visitExceptionEntry(exceptionIndex);
		}
	}

	template<typename TWriter>
	struct Writer {
		TWriter& w;

		Writer(TWriter& w) :
				w(w) {
		}
		void visitExceptionCount(u2 count) {
			w.writeu2(count);
		}

		void visitExceptionEntry(u2 exceptionIndex) {
			w.writeu2(exceptionIndex);
		}
	};

	template<typename TVisitor>
	struct Forward {
		TVisitor& av;

		Forward(TVisitor& av) :
				av(av) {
		}

		void visitExceptionCount(u2 count) {
			av.visitExceptionCount(count);
		}

		void visitExceptionEntry(u2 exceptionIndex) {
			av.visitExceptionEntry(exceptionIndex);
		}
	};
};

}

#endif
