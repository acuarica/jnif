/*
 * Profiler.hpp
 *
 *  Created on: Jun 7, 2014
 *      Author: luigi
 */

#ifndef PROFILER_HPP
#define PROFILER_HPP

#include <jnif.hpp>

#include <time.h>
#include <ctime>
#include <sys/time.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

class Profiler {
public:

	Profiler(FILE** profLog, int tid, const char* runId) :
			_tid(tid), _profLog(profLog), _runId(runId) {
	}

	void open() {
		if (*_profLog != NULL) {
			return;
		}

		std::stringstream ss;
		ss << args.profPath << ".tid-" << _tid << ".prof";

		*_profLog = fopen(ss.str().c_str(), "w+");

		if (*_profLog == NULL) {
			perror("Unable to create prof file");
			exit(1);
		}
	}

	void prof(const char* className, double time) {
		open();
		fprintf(*_profLog, "%s,%s,%f\n", _runId, className, time);
	}

private:
	int _tid;
	FILE** _profLog;
	const char* _runId;
};

class ProfEntry {
public:

	ProfEntry(const Profiler& p, const char* entryName) :
			_p(p), _entryName(entryName), _start(getTime()) {
	}

	~ProfEntry() {
		double end = getTime();
		double instrTime = end - _start;
		_p.prof(_entryName, instrTime);
	}

	static double getTime() {
#ifdef __MACH__
		host_name_port_t self = mach_host_self();
		clock_serv_t cclock;
		host_get_clock_service(self, REALTIME_CLOCK, &cclock);
		mach_timespec_t ts;
		clock_get_time(cclock, &ts);
		mach_port_deallocate(self, cclock);
#else
		struct timespec ts;
		clock_gettime( CLOCK_MONOTONIC, &ts );
#endif
		return ts.tv_sec + ts.tv_nsec * 1e-9;
	}
private:

	Profiler _p;
	const char* _entryName;
	double _start;

};

#endif
