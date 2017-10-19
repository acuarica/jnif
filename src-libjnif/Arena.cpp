/*
 * Arena.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */
#include <jnif.hpp>

namespace jnif {

    class Arena::Block {
    public:

        Block(Block* next, size_t blockSize) :
                _next(next), _buffer(malloc(blockSize)), _position(0) {
            JnifError::check(_buffer != nullptr, "Block alloc is NULL");
        }

        ~Block() {
            free(_buffer);
        }

        void* alloc(int size) {
            JnifError::assert(size <= BLOCK_SIZE, "Size too large for a block: ", size);

            if (_position + size <= BLOCK_SIZE) {
                void* offset = (char*) _buffer + _position;
                _position += size;
                return offset;
            }

            return nullptr;
        }

        Block* _next;
        void* _buffer;
        int _position;
    };

    Arena::Arena(size_t blockSize) :
            blockSize(blockSize),
            _head(new Block(nullptr, blockSize)) {
    }

    Arena::~Arena() {
        for (Block* block = _head; block != nullptr;) {
            Block* next = block->_next;
            delete block;
            block = next;
        }
    }

    void* Arena::alloc(int size) {
        void* res = _head->alloc(size);
        if (res == nullptr) {
            _head = new Block(_head, blockSize);
            res = _head->alloc(size);
        }

        JnifError::assert(res != nullptr, "alloc == NULL");

        return res;
    }

}
