#ifndef __VTX_ACTION_MANAGER__
#define __VTX_ACTION_MANAGER__

#include "base_action_undonable.hpp"
#include "define.hpp"
#include "generic/base_updatable.hpp"
#include <functional>
#include <list>
#include <queue>
#include <string>

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

			ActionManager()						   = default;
			ActionManager( const ActionManager & ) = delete;
			ActionManager & operator=( const ActionManager & ) = delete;
			~ActionManager()								   = default;

			void _flushAction( BaseAction * const );
			void _purgeBuffer();
		};
	} // namespace Action

	inline void VTX_ACTION( VTX::Action::BaseAction * const p_action )
	{
		Action::ActionManager::get().execute( p_action );
	}

	inline void VTX_ACTION_ENQUEUE( VTX::Action::BaseAction * const p_action )
	{
		Action::ActionManager::get().enqueue( p_action );
	}

	inline void VTX_ACTION( const std::string & p_action ) { Action::ActionManager::get().execute( p_action ); }
} // namespace VTX
#endif
