#ifndef __VTX_ACTION_MANAGER__
#define __VTX_ACTION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "base_action_undonable.hpp"
#include <list>
#include <string>

namespace VTX
{
	namespace Action
	{
		class ActionManager
		{
		  public:
			void executeAction( BaseAction * const );
			void executeAction( const std::string & );
			bool canUndo() const;
			void undo();
			bool canRedo() const;
			void redo();

		  private:
			std::list<BaseActionUndonable *> _bufferUndo = std::list<BaseActionUndonable *>();
			std::list<BaseActionUndonable *> _bufferRedo = std::list<BaseActionUndonable *>();

			void _purgeBuffer( BaseActionUndonable * const );
		};
	} // namespace Action
} // namespace VTX
#endif
