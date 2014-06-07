/*
 * tests.hpp
 *
 *  Created on: Jun 2, 2014
 *      Author: luigi
 */

#ifndef TESTS_HPP
#define TESTS_HPP

#include "apply.hpp"

void testPrinter(const JavaFile& jf);
void testSize(const JavaFile& jf);
void testWriter(const JavaFile& jf);
void testAnalysis(const JavaFile& jf);
void testAnalysisPrinter(const JavaFile& jf);
void testAnalysisWriter(const JavaFile& jf);
void testNopAdderInstrPrinter(const JavaFile& jf);
void testNopAdderInstrSize(const JavaFile& jf);
void testNopAdderInstrWriter(const JavaFile& jf);
void testNopAdderInstrAnalysisPrinter(const JavaFile& jf);
void testNopAdderInstrAnalysisWriter(const JavaFile& jf);

#endif
