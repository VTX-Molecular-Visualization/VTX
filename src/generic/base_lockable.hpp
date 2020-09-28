#ifndef __VTX_BASE_LOCKABLE__
#define __VTX_BASE_LOCKABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include <iostream>
#include <mutex>

namespace VTX
{
	namespace Generic
	{
		class BaseLocakble
		{
		  protected:
			std::mutex _mutex;

			inline void _lock() { _mutex.lock(); }

			inline void _unlock() { _mutex.unlock(); }
		};
	} // namespace Generic
} // namespace VTX
#endif
