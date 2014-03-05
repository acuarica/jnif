#ifndef JNIF_LNTATTR_HPP
#define JNIF_LNTATTR_HPP

#include "../base.hpp"

namespace jnif {

/**
 * Represents the LineNumberTable attribute within the Code attribute.
 */
struct LntAttr: Attr {

	LntAttr(u2 nameIndex) :
			Attr(nameIndex) {
	}

	virtual ~LntAttr() {
	}

	struct LnEntry {
		u2 startpc;
		u2 lineno;
	};

	std::vector<LnEntry> lnt;

	virtual void write(BufferWriter& bw) {
		u2 count = lnt.size();

		bw.writeu2(count);

		for (u4 i = 0; i < count; i++) {
			LnEntry& lne = lnt[i];

			bw.writeu2(lne.startpc);
			bw.writeu2(lne.lineno);
		}
	}
};

}

#endif
