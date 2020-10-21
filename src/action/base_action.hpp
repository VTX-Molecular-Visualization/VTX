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

		template<typename P1>
		class BaseActionParametrized : public BaseAction
		{
		  public:
			BaseActionParametrized( P1 & p_parameter ) { _parameter = &p_parameter; };

		  protected:
			P1 * _parameter;
		};
	} // namespace Action
} // namespace VTX
#endif
