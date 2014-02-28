#ifndef JNIF_ATTR_HPP
#define JNIF_ATTR_HPP

#include "../base.hpp"

namespace jnif {

class Attr {
	Attr(const Attr&) = delete;
public:

	virtual ~Attr() {
	}

	u2 nameIndex;
	u4 len;

	virtual void write(BufferWriter& bw) = 0;
protected:
	Attr(u2 nameIndex, u4 len = 0) :
			nameIndex(nameIndex), len(len) {
	}

};

}

#endif
