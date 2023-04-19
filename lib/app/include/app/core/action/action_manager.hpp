#ifndef __VTX_CORE_ACTION_ACTION_MANAGER__
#define __VTX_CORE_ACTION_ACTION_MANAGER__

#include "app/core/action/base_action.hpp"
#include "app/core/action/base_action_undonable.hpp"
#include "app/old_app/generic/base_updatable.hpp"
#include <functional>
#include <list>
#include <queue>
#include <string>

namespace VTX
{
	namespace Core::Action
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

			void		execute( BaseAction * const );
			void		execute( const std::string & );
			inline void enqueue( BaseAction * const p_action ) { _actionQueue.emplace( p_action ); }
			bool		canUndo() const;
			void		undo();
			bool		canRedo() const;
			void		redo();

			virtual void update( const float & p_deltaTime ) override;

		  private:
			ListActionUndonablePtr _bufferUndo	= ListActionUndonablePtr();
			ListActionUndonablePtr _bufferRedo	= ListActionUndonablePtr();
			QueueVTXActionPtr	   _actionQueue = QueueVTXActionPtr();

			ActionManager()									   = default;
			ActionManager( const ActionManager & )			   = delete;
			ActionManager & operator=( const ActionManager & ) = delete;
			~ActionManager()								   = default;

			void _flushAction( BaseAction * const );
			void _purgeBuffer();
		};
	} // namespace Core::Action

	inline void VTX_ACTION( Core::Action::BaseAction * const p_action )
	{
		Core::Action::ActionManager::get().execute( p_action );
	}

	inline void VTX_ACTION_ENQUEUE( Core::Action::BaseAction * const p_action )
	{
		Core::Action::ActionManager::get().enqueue( p_action );
	}

	inline void VTX_ACTION( const std::string & p_action ) { Core::Action::ActionManager::get().execute( p_action ); }
} // namespace VTX
#endif
