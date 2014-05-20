/*
 * InstList.cpp
 *
 *  Created on: May 16, 2014
 *      Author: luigi
 */
#include "jnif.hpp"
#include "jnifex.hpp"

namespace jnif {

Inst* InstList::Iterator::operator*() {
	Error::assert(position != nullptr, "Dereferencing * on nullptr");
	return position;
}

Inst* InstList::Iterator::operator->() const {
	Error::assert(position != nullptr, "Dereferencing -> on nullptr");

	return position;
}

InstList::Iterator& InstList::Iterator::operator++() {
	Error::assert(position != nullptr, "Doing ++ at nullptr");
	position = position->next;

	return *this;
}

InstList::Iterator& InstList::Iterator::operator--() {
	if (position == nullptr) {
		position = last;
	} else {
		position = position->prev;
	}

	Error::assert(position != nullptr, "Doing -- at nullptr after last");

	return *this;
}

InstList::~InstList() {
	for (Inst* inst = first; inst != nullptr;) {
		Inst* next = inst->next;
		delete inst;
		inst = next;
	}
}

void InstList::addInst(Inst* inst, Inst* pos) {
	Error::assert((first == nullptr) == (last == nullptr),
			"Invalid head/tail/size: head: ", first, ", tail: ", last,
			", size: ", size);

	Error::assert((first == nullptr) == (size == 0),
			"Invalid head/tail/size: head: ", first, ", tail: ", last,
			", size: ", size);

	Inst* p;
	Inst* n;
	if (first == nullptr) {
		Error::assert(pos == nullptr, "Invalid pos");

		p = nullptr;
		n = nullptr;
		first = inst;
		last = inst;
	} else {
		if (pos == nullptr) {
			p = last;
			n = nullptr;
			last = inst;
		} else {
			p = pos->prev;
			n = pos;
		}
	}

	inst->prev = p;
	inst->next = n;

	if (inst->prev != nullptr) {
		inst->prev->next = inst;
	}

	if (inst->next != nullptr) {
		inst->next->prev = inst;
	}

	size++;
}

}
