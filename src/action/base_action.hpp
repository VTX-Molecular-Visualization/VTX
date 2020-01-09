#ifndef __VTX_BASE_ACTION__
#define __VTX_BASE_ACTION__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Action
	{
		class BaseAction
		{
		  public:
			virtual void run() = 0;
		};
	} // namespace Action
} // namespace VTX
#endif
