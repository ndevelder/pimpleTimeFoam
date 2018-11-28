#pragma once

#include <ctime>

class StopWatch
{
	private:
		std::clock_t startTime;
		double totalTime;
		double indTime;
	public:
		StopWatch()
		{
			totalTime = 0;
		}
		void start()
		{
			startTime = std::clock();
		}
		void stop()
		{
			indTime = ( std::clock() - startTime ) / (double) CLOCKS_PER_SEC;
			totalTime += ( std::clock() - startTime ) / (double) CLOCKS_PER_SEC;
		}
		double getTotalTime()
		{
			return totalTime;
		}
		double getIndTime()
		{
			return indTime;
		}
};
