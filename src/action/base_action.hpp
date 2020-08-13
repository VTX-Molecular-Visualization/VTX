#ifndef __VTX_BASE_ACTION__
#define __VTX_BASE_ACTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "exception.hpp"
#include <string>
#include <vector>

namespace VTX
{
	namespace Action
	{
		class BaseAction
		{
		  public:
			virtual void execute() = 0;
			virtual void displayUsage() {}
		};
	} // namespace Action
} // namespace VTX
#endif
