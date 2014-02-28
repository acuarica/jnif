#ifndef JNIF_EXCEPTIONSATTR_HPP
#define JNIF_EXCEPTIONSATTR_HPP

#include <vector>

#include "../base.hpp"

namespace jnif {

/**
 * Represents the Exceptions attribute.
 */
class ExceptionsAttr: public Attr {
public:
	ExceptionsAttr(u2 nameIndex, u4 len, const vector<u2>& es) :
			Attr(nameIndex, len), es(es) {
	}

	virtual ~ExceptionsAttr() {
	}

	vector<u2> es;

	virtual void write(BufferWriter& bw) {
		u2 size = es.size();

		bw.writeu2(size);
		for (u4 i = 0; i < size; i++) {
			u2 e = es[i];
			bw.writeu2(e);
		}
	}

};

}

#endif
