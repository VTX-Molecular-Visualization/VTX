#ifndef __VTX_APP_OLD_MANAGER_ACTION_MANAGER__
#define __VTX_APP_OLD_MANAGER_ACTION_MANAGER__

#include "app/old/application/generic/base_updatable.hpp"
#include "app/old/core/action/base_action.hpp"
#include "app/old/core/action/base_action_undonable.hpp"
#include <functional>
#include <list>
#include <queue>
#include <string>

namespace VTX
{
	namespace App::Old::Manager
	{
		using namespace VTX::App::Old::Core::Action;

		class ActionManager final : public Application::Generic::BaseUpdatable
		{
		  public:
			using ListActionUndonablePtr = std::list<BaseActionUndonable *>;
			using QueueVTXActionPtr		 = std::queue<BaseAction *>;

			inline static ActionManager & get()
			{
				static ActionManager instance;
				return instance;
			}

			// TODO Use shared_ptr instead of BaseAction * const
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
	} // namespace App::Old::Manager

} // namespace VTX
#endif
