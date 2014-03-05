#ifndef JNIF_SOURCEFILEATTR_HPP
#define JNIF_SOURCEFILEATTR_HPP

#include "../base.hpp"

namespace jnif {

/**
 *
 */
class SourceFileAttr: public Attr {
public:
	SourceFileAttr(u2 nameIndex, u4 len, u2 sourceFileIndex) :
			Attr(nameIndex, len), sourceFileIndex(sourceFileIndex) {
	}

	virtual ~SourceFileAttr() {
	}

	const u2 sourceFileIndex;

	virtual void accept(Visitor& av) {
		av.visitSourceFile(*this);
	}

	virtual void write(BufferWriter& bw) {
		bw.writeu2(sourceFileIndex);
	}
};

}

#endif
