#ifndef JNIF_CODEATTR_HPP
#define JNIF_CODEATTR_HPP

#include <sstream>

#include "../base.hpp"

namespace jnif {

/**
 * Represent the Code attribute of a method.
 */
class CodeAttr: public Attr {
public:

	struct ExceptionEntry {
		u2 startpc;
		u2 endpc;
		u2 handlerpc;
		u2 catchtype;
	};

	CodeAttr(u2 nameIndex) :
			Attr(nameIndex), maxStack(0), maxLocals(0) {
	}

	virtual ~CodeAttr() {
	}

	u2 maxStack;
	u2 maxLocals;

	std::basic_stringstream<u1> codeBuffer;

	std::vector<ExceptionEntry> exceptions;

	Attrs attrs;

	virtual void write(BufferWriter& bw) {
		bw.writeu2(maxStack);
		bw.writeu2(maxLocals);

		u4 codeLen = codeBuffer.tellp();
		bw.writeu4(codeLen);

		u1* codeStart = bw.pos();
		bw.skip(codeLen);
		{
			BufferWriter bw(codeStart, codeLen);
			const std::basic_string<u1>& str = codeBuffer.str();
			bw.writecount(str.c_str(), codeLen);
		}

		u2 esize = exceptions.size();
		bw.writeu2(esize);
		for (u4 i = 0; i < esize; i++) {
			ExceptionEntry& e = exceptions[i];
			bw.writeu2(e.startpc);
			bw.writeu2(e.endpc);
			bw.writeu2(e.handlerpc);
			bw.writeu2(e.catchtype);
		}

		attrs.writeAttrs(bw);
	}
};

}

#endif
