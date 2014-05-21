/*
 * testagent.hpp
 *
 *  Created on: May 21, 2014
 *      Author: luigi
 */
#ifndef TESTAGENT_HPP
#define TESTAGENT_HPP

#include <jnif.hpp>

class Options {
public:

	jnif::String instrFuncName;
	jnif::String outputPath;
};

extern Options args;

#endif
