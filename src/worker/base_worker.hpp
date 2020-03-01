#ifndef __VTX_BASE_WORKER__
#define __VTX_BASE_WORKER__

#ifdef _MSC_VER
#pragma once
#endif

#include <functional>
#include <thread>

namespace VTX
{
	namespace Worker
	{
		class BaseWorker
		{
		  public:
			virtual void work() = 0;
		};
	} // namespace Worker
} // namespace VTX
#endif
