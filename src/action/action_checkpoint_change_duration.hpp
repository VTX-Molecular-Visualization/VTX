#ifndef __VTX_ACTION_CHECKPOINT_CHANGE_DURATION__
#define __VTX_ACTION_CHECKPOINT_CHANGE_DURATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/model_checkpoint.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionCheckpointChangeDuration : public BaseAction
		{
		  public:
			explicit ActionCheckpointChangeDuration( const float p_duration ) : _duration( p_duration ) {}

			virtual void execute() override {}

		  private:
			const float _duration;
		};
	} // namespace Action
} // namespace VTX
#endif
