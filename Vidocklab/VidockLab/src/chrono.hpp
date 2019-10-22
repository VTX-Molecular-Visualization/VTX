/*
 * File: chrono.hpp
 * Author: Maxime MARIA
 */

#ifndef __CHRONO_HPP__
#define __CHRONO_HPP__

#include <chrono>

namespace vidocklab
{
	class Chrono
	{
		using Clock = std::chrono::high_resolution_clock;
		using Duration = std::chrono::duration<double>;

	public:
		void start()
		{
			begin = Clock::now();
		}
		void stop()
		{
			end = Clock::now();
		}
		double elapsedTime() const
		{
			return (std::chrono::duration_cast<Duration>(end - begin)).count();
		}

	private:
		Clock::time_point begin;
		Clock::time_point end;
	};
} // namespace vidocklab

#endif // __CHRONO_HPP__
