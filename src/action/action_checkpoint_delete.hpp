#ifndef __VTX_ACTION_CHECKPOINT_DELETE__
#define __VTX_ACTION_CHECKPOINT_DELETE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/checkpoint.hpp"
#include "model/path.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionCheckpointDelete : public BaseAction
		{
		  public:
			explicit ActionCheckpointDelete( Model::Checkpoint & p_checkpoint ) : _checkpoint( p_checkpoint ) {}

			virtual void execute() override
			{
				_checkpoint.getPathPtr()->resetSelectedCheckpoint();
				_checkpoint.getPathPtr()->removeCheckpoint( &_checkpoint );
				delete &_checkpoint;
			}

		  private:
			Model::Checkpoint & _checkpoint;
		};
	} // namespace Action
} // namespace VTX
#endif
