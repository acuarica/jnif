/*
 * tests.hpp
 *
 *  Created on: Jun 2, 2014
 *      Author: luigi
 */

#ifndef TESTS_HPP
#define TESTS_HPP

#include "apply.hpp"

void testPrinter(const JavaFile& jf, jnif::ClassFile& cf);
void testSize(const JavaFile& jf, jnif::ClassFile& cf);
void testWriter(const JavaFile& jf, jnif::ClassFile& cf);
void testAnalysis(const JavaFile& jf, jnif::ClassFile& cf);
void testAnalysisPrinter(const JavaFile& jf, jnif::ClassFile& cf);
void testAnalysisWriter(const JavaFile& jf, jnif::ClassFile& cf);
void testNopAdderInstrPrinter(const JavaFile& jf, jnif::ClassFile& cf);
void testNopAdderInstrSize(const JavaFile& jf, jnif::ClassFile& cf);
void testNopAdderInstrWriter(const JavaFile& jf, jnif::ClassFile& cf);
void testNopAdderInstrAnalysisPrinter(const JavaFile& jf, jnif::ClassFile& cf);
void testNopAdderInstrAnalysisWriter(const JavaFile& jf, jnif::ClassFile& cf);

#endif
