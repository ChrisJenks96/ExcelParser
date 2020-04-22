#include "Time.h"

void PerformanceCounter::StartCounter()
{
	QueryPerformanceFrequency(&li);
	PCFreq = (double)li.QuadPart / 1000.0;
	QueryPerformanceCounter(&li);
	counterStart = li.QuadPart;
}

double PerformanceCounter::GetCounter()
{
	QueryPerformanceCounter(&li);
	return (double)(li.QuadPart - counterStart) / PCFreq;
}