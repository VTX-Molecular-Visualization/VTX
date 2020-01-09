#ifndef __VTX_ACTION_SNAPSHOT__
#define __VTX_ACTION_SNAPSHOT__

#ifdef _MSC_VER
#pragma once
#endif

#include "../tool/snapshoter.hpp"
#include "base_action.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionSnapshot : public BaseAction
		{
		  public:
			virtual void execute() override
			{
				Tool::Snapshoter snapshoter;
				snapshoter.takeSnapshot();
			};
		};
	} // namespace Action
} // namespace VTX
#endif
