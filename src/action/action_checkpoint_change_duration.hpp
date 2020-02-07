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
			explicit ActionCheckpointChangeDuration( Model::ModelCheckpoint & p_checkpoint, const float p_duration ) :
				_checkpoint( p_checkpoint ), _duration( p_duration )
			{
			}

			virtual void execute() override { _checkpoint.setDuration( _duration ); }

		  private:
			Model::ModelCheckpoint & _checkpoint;
			const float				 _duration;
		};
	} // namespace Action
} // namespace VTX
#endif
