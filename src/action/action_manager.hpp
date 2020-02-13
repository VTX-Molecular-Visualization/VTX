#ifndef __VTX_ACTION_MANAGER__
#define __VTX_ACTION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action_undonable.hpp"
#include "define.hpp"
#include <list>
#include <string>

namespace VTX
{
	namespace Action
	{
		class ActionManager
		{
		  public:
			using ListActionUndonablePtr = std::list<BaseActionUndonable *>;

			void executeAction( BaseAction * const );
			void executeAction( const std::string & );
			bool canUndo() const;
			void undo();
			bool canRedo() const;
			void redo();

		  private:
			ListActionUndonablePtr _bufferUndo = ListActionUndonablePtr();
			ListActionUndonablePtr _bufferRedo = ListActionUndonablePtr();

			void _purgeBuffer();
		};
	} // namespace Action
} // namespace VTX
#endif
