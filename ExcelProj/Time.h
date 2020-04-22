#ifndef TIME_H
#define TIME_H

#include <Windows.h>

class PerformanceCounter
{
	public:
		PerformanceCounter() { PCFreq = 0.0; counterStart = 0; }
		void StartCounter();
		double GetCounter();
		~PerformanceCounter() {}
	private:
		LARGE_INTEGER li;
		double PCFreq;
		long long counterStart;
};

#endif
