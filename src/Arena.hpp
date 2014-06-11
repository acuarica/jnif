/*
 * Arena.hpp
 *
 *  Created on: Jun 11, 2014
 *      Author: luigi
 */

#ifndef JNIF_ARENA_HPP
#define JNIF_ARENA_HPP

namespace jnif {

class Arena {
public:

	Arena();
	~Arena();

	void* alloc(int size);

	template<typename T, typename ... TArgs>
	T* create(const TArgs& ... args);

private:

	class Block;

	Block* _head;

};

}

#endif
