#ifndef JNIF_UNKNOWNATTR_HPP
#define JNIF_UNKNOWNATTR_HPP

#include "../base.hpp"

namespace jnif {

/**
 *
 */
class UnknownAttr: public Attr {
public:

	UnknownAttr(u2 nameIndex, u4 len, const u1* data) :
			Attr(nameIndex, len), data(data) {
	}

	virtual ~UnknownAttr() {
	}

	virtual void write(BufferWriter& bw) {
		bw.writecount(data, len);
	}

	const u1 * const data;
};

}

#endif
