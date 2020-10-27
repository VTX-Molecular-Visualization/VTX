#ifndef __VTX_ACTION_MANAGER__
#define __VTX_ACTION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action_undonable.hpp"
#include "define.hpp"
#include "generic/base_updatable.hpp"
#include <functional>
#include <list>
#include <queue>
#include <string>

#define DELAY_ACTIONS

namespace VTX
{
	namespace Action
	{
		class ActionManager final : public Generic::BaseUpdatable
		{
		  public:
			using ListActionUndonablePtr = std::list<BaseActionUndonable *>;
			using QueueVTXActionPtr		 = std::queue<BaseAction *>;

			inline static ActionManager & get()
			{
				static ActionManager instance;
				return instance;
			}

			void execute( BaseAction * const, const bool = false );
			void execute( const std::string &, const bool = false );
			bool canUndo() const;
			void undo();
			bool canRedo() const;
			void redo();

			virtual void update( const double & p_deltaTime ) override;

		  private:
			ListActionUndonablePtr _bufferUndo	= ListActionUndonablePtr();
			ListActionUndonablePtr _bufferRedo	= ListActionUndonablePtr();
			QueueVTXActionPtr	   _actionQueue = QueueVTXActionPtr();

			ActionManager()						   = default;
			ActionManager( const ActionManager & ) = delete;
			ActionManager & operator=( const ActionManager & ) = delete;
			~ActionManager()								   = default;

			void _flushAction( BaseAction * );
			void _purgeBuffer();
		};
	} // namespace Action

	inline void VTX_ACTION( VTX::Action::BaseAction * const p_action, const bool p_force = false ) { Action::ActionManager::get().execute( p_action, p_force ); }
	inline void VTX_ACTION( const std::string & p_action, const bool p_force = false ) { Action::ActionManager::get().execute( p_action, p_force ); }
} // namespace VTX
#endif
