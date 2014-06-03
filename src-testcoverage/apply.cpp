/*
 * apply.cpp
 *
 *  Created on: Jun 2, 2014
 *      Author: luigi
 */
#include "apply.hpp"

using namespace std;
using namespace jnif;

void apply(ostream& os, const std::list<JavaFile>& tests, TestFunc instr) {
	int i = 0;
	for (const JavaFile& jf : tests) {
		ClassFile cf(jf.data, jf.len);

		try {
			instr(jf, cf);
		} catch (JnifException& ex) {
			os << "Error: Exception on class " << jf.name << endl;
			throw ex;
		}

		i++;
		if (i % 1000 == 0) {
			os << ".";
		}
	}
}
