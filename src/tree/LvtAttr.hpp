#ifndef JNIF_LVTATTR_HPP
#define JNIF_LVTATTR_HPP

#include "../base.hpp"

namespace jnif {

/**
 * Represents the LineNumberTable attribute within the Code attribute.
 */
struct LvtAttr: Attr {

	LvtAttr(u2 nameIndex) :
			Attr(nameIndex) {
	}

	virtual ~LvtAttr() {
	}

	struct LvEntry {
		u2 startPc;
		u2 len;
		u2 varNameIndex;
		u2 varDescIndex;
		u2 index;
	};


	std::vector<LvEntry> lvt;

	virtual void write(BufferWriter& bw) {
		u2 count = lvt.size();

		bw.writeu2(count);

		for (u4 i = 0; i < count; i++) {
			LvEntry& lve = lvt[i];

			bw.writeu2(lve.startPc);
			bw.writeu2(lve.len);
			bw.writeu2(lve.varNameIndex);
			bw.writeu2(lve.varDescIndex);
			bw.writeu2(lve.index);
		}
	}

};

}

#endif
