#ifndef JNIF_EXCEPTIONSATTRPARSER_HPP
#define JNIF_EXCEPTIONSATTRPARSER_HPP

#include "../base.hpp"
#include "../tree/ConstPool.hpp"

namespace jnif {

/**
 * Exceptions
 */
class ExceptionsAttrParser {
public:

	static constexpr const char* AttrName = "Exceptions";

	template<typename TReader, typename TVisitor>
	inline static void parse(TReader& br, TVisitor& v, ConstPool& cp,
			u2 nameIndex) {
		u2 len = br.readu2();

		v.visitExceptionCount(len);

		for (int i = 0; i < len; i++) {
			u2 exceptionIndex = br.readu2();

			v.visitExceptionEntry(exceptionIndex);
		}
	}

	template<typename TWriter>
	class Writer {
	public:
		inline Writer(TWriter& w) :
				w(w) {
		}
		inline void visitExceptionCount(u2 count) {
			w.writeu2(count);
		}

		inline void visitExceptionEntry(u2 exceptionIndex) {
			w.writeu2(exceptionIndex);
		}

	private:
		TWriter& w;

	};
};

}

#endif
