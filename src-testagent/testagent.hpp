/*
 * testagent.hpp
 *
 *  Created on: May 21, 2014
 *      Author: luigi
 */
#ifndef TESTAGENT_HPP
#define TESTAGENT_HPP

#include <string>

class Options {
public:

	std::string instrFuncName;
	std::string profPath;
	std::string outputPath;
	std::string runId;

};

extern Options args;

#endif
