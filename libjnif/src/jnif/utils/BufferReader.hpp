#ifndef JNIF_BUFFERREADER_HPP
#define JNIF_BUFFERREADER_HPP

#include "../base.hpp"

namespace jnif {

/**
 * Implements a memory buffer reader in big-endian encoding.
 */
class BufferReader {
public:

	inline BufferReader(const u1* buffer, int len) :
			buffer(buffer), len(len), off(0) {
	}

	inline ~BufferReader() {
		end();
	}

	inline u1 readu1() {
		ASSERT(off + 1 <= len, "Invalid read");

		u1 result = buffer[off];

		off += 1;

		return result;
	}

	inline u2 readu2() {
		ASSERT(off + 2 <= len, "Invalid read 2");

		u1 r0 = buffer[off + 0];
		u1 r1 = buffer[off + 1];

		u2 result = r0 << 8 | r1;

		off += 2;

		return result;
	}

	inline u4 readu4() {
		ASSERT(off + 4 <= len, "Invalid read 4");

		u1 r0 = buffer[off + 0];
		u1 r1 = buffer[off + 1];
		u1 r2 = buffer[off + 2];
		u1 r3 = buffer[off + 3];

		u4 result = r0 << 24 | r1 << 16 | r2 << 8 | r3;

		off += 4;

		return result;
	}

	inline void skip(int count) {
		ASSERT(off + count <= len, "Invalid read count: %d (offset: %d)", count,
				off);

		off += count;
	}

	inline int offset() const {
		return off;
	}

	inline const u1* pos() const {
		return buffer + off;
	}

	inline bool eor() const {
		return off == len;
	}

private:

	inline void end() {
		ASSERT(off == len,
				"End of buffer not reached while expecting end of buffer");
	}

	const u1 * const buffer;
	const int len;
	int off;
};

}

#endif
