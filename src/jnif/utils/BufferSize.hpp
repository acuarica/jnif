#ifndef JNIF_BUFFERSIZE_HPP
#define JNIF_BUFFERSIZE_HPP

#include "../base.hpp"

namespace jnif {

class BufferSize {
public:
	inline BufferSize() :
			offset(0) {
	}

	inline void writeu1(u1 value) {
		offset += 1;
	}

	inline void writeu2(u2 value) {
		offset += 2;
	}

	inline void writeu4(u4 value) {
		offset += 4;
	}

	inline void writecount(const void* source, int count) {
		offset += count;
	}

	inline u1* pos() {
		return NULL;
	}

	inline void skip(int count) {
		offset += count;
	}

	inline int offset2() {
		return offset;
	}

	inline int size()  {
		return offset;
	}

private:

	int offset;

};

}

#endif
