#ifndef JNIF_ATTR_HPP
#define JNIF_ATTR_HPP

#include "../base.hpp"

namespace jnif {

class Visitor;

struct Attr {
	Attr(const Attr&) = delete;

	virtual ~Attr() {
	}

	u2 nameIndex;
	u4 len;

	virtual void accept(Visitor& av) = 0;

	virtual void write(BufferWriter& bw) = 0;
protected:
	Attr(u2 nameIndex, u4 len = 0) :
			nameIndex(nameIndex), len(len) {
	}

};

}

#endif
