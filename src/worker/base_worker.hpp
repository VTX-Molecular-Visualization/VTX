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
			virtual ~BaseWorker()	   = default;
			virtual void		work() = 0;
			inline const bool	isFinished() const { return _isFinished; }
			virtual const float getProgress() const { return _progress; }

		  protected:
			bool  _isFinished = false;
			float _progress	  = 0.f; // The thread % [0-1]
		};
	} // namespace Worker
} // namespace VTX
#endif
