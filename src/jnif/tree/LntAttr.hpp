#ifndef JNIF_LNTATTR_HPP
#define JNIF_LNTATTR_HPP

#include "../base.hpp"

namespace jnif {

/**
 * Represents the LineNumberTable attribute within the Code attribute.
 */
class LntAttr: Attr {
public:
	LntAttr(u2 nameIndex) :
			Attr(nameIndex) {
	}

	virtual ~LntAttr() {
	}

	struct Ln {
		u2 startpc;
		u2 lineno;
	};

	std::vector<Ln> lns;

	virtual void write(BufferWriter& bw) {
		u2 size = lns.size();
		bw.writeu2(size);

		for (u4 i = 0; i < size; i++) {
			Ln& ln = lns[i];

			bw.writeu2(ln.startpc);
			bw.writeu2(ln.lineno);
		}
	}

};

}

#endif
