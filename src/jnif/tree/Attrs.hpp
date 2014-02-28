#ifndef JNIF_ATTRS_HPP
#define JNIF_ATTRS_HPP

#include "../base.hpp"
#include <vector>

namespace jnif {

/**
 * Represents a collection of attributes within a class, method or field
 * or even with another attributes, e.g., CodeAttr.
 */
class Attrs {
	Attrs(const Attrs&) = delete;

public:
	Attrs(Attrs&&) = default;
	inline Attrs() {
	}

	inline Attr* add(Attr* attr) {
		attrs.push_back(attr);

		return attr;
	}

	inline u2 size() const {
		return attrs.size();
	}

	inline const Attr& operator[](u2 index) const {
		return *attrs[index];
	}

	void writeAttrs(BufferWriter& bw) {
		bw.writeu2(size());

		for (u4 i = 0; i < size(); i++) {
			Attr& attr = *attrs[i];

			bw.writeu2(attr.nameIndex);
			bw.writeu4(attr.len);

			u1* pos = bw.pos();
			bw.skip(attr.len);
			{
				BufferWriter bw(pos, attr.len);
				attr.write(bw);
			}
		}
	}

private:

	vector<Attr*> attrs;
};

}

#endif
