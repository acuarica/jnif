/*
 * Arena.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */
#include "Arena.hpp"
//#include "Error.hpp"
#include <stdlib.h>

#define BLOCK_SIZE (1024*1024)

namespace jnif {

class Arena::Block {
public:

	Block(Block* next) :
			_next(next), _buffer(malloc(BLOCK_SIZE)), _position(0) {
	}

	~Block() {
		free(_buffer);
	}

	void* alloc(int size) {
		if (_position + size <= (BLOCK_SIZE)) {
			void* offset = (char*) _buffer + _position;
			_position += size;
			return offset;
		}

		return NULL;
	}

	Block* _next;
	void* _buffer;
	int _position;
};

Arena::Arena() :
		_head(new Block(NULL)) {
}

Arena::~Arena() {
	for (Block* block = _head; block != NULL;) {
		free(block->_buffer);
		Block* next = block->_next;
		free(block);
		block = next;
	}
}

void* Arena::alloc(int size) {
	void* res = _head->alloc(size);
	if (res == NULL) {
		_head = new Block(_head);
		res = _head->alloc(size);
	}

	//Error::assert(res != NULL, "alloc == NULL");

	return res;
}

template<typename T, typename ... TArgs>
T* Arena::create(const TArgs& ... args) {
	void* buf = alloc(sizeof(T));
	return new (buf) T(args ...);
}

}
