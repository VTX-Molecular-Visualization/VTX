#ifndef __VTX_ACTION_MANAGER__
#define __VTX_ACTION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action_undonable.hpp"
#include "define.hpp"
#include "generic/base_updatable.hpp"
#include <list>
#include <queue>
#include <string>

namespace VTX
{
	namespace Action
	{
		class ActionManager : public Generic::BaseUpdatable
		{
		  public:
			using ListActionUndonablePtr = std::list<BaseActionUndonable *>;
			using QueueVTXActionPtr		 = std::queue<BaseAction *>;

			void execute( BaseAction * const );
			void execute( const std::string & );
			bool canUndo() const;
			void undo();
			bool canRedo() const;
			void redo();

			virtual void update( const double p_deltaTime ) override;

		  private:
			ListActionUndonablePtr _bufferUndo	= ListActionUndonablePtr();
			ListActionUndonablePtr _bufferRedo	= ListActionUndonablePtr();
			QueueVTXActionPtr	   _actionQueue = QueueVTXActionPtr();

			void _flushAction( BaseAction * );
			void _purgeBuffer();
		};
	} // namespace Action
} // namespace VTX
#endif
