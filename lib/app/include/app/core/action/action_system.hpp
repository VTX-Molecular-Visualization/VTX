#ifndef __VTX_APP_CORE_ACTION_ACTION_SYSTEM__
#define __VTX_APP_CORE_ACTION_ACTION_SYSTEM__

#include "app/core/action/base_action.hpp"
#include "app/core/action/base_action_undonable.hpp"
#include <concepts>
#include <list>
#include <memory>
#include <util/concepts.hpp>
#include <util/singleton.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Action
{
	template<typename T>
	concept UndonableActionConcept = std::derived_from<T, Core::Action::BaseActionUndonable>;
	template<typename T>
	concept ActionConcept = std::derived_from<T, Core::Action::BaseAction> && ( !UndonableActionConcept<T> );

	// Action manager is a system used to execute all the actions in VTX.
	// Action manager must be used in order to handle undonable actions.
	class ActionSystem
	{
	  public:
		using BaseAction		  = Core::Action::BaseAction;
		using BaseActionUndonable = Core::Action::BaseActionUndonable;

		using ListActionUndonablePtr = std::list<std::unique_ptr<BaseActionUndonable>>;
		// using QueueVTXActionPtr		 = std::queue<BaseAction *>;

	  public:
		ActionSystem();
		~ActionSystem();

		// Execute function A with args Args
		template<ActionConcept A, typename... Args>
		void execute( const Args &... p_args )
		{
			std::unique_ptr<BaseAction> actionPtr = std::make_unique<A>( p_args... );
			execute( actionPtr );
		}
		template<UndonableActionConcept A, typename... Args>
		void execute( const Args &... p_args )
		{
			std::unique_ptr<BaseActionUndonable> actionPtr = std::make_unique<A>( p_args... );
			execute( actionPtr );
		}

		void execute( std::unique_ptr<BaseAction> & p_action );
		void execute( std::unique_ptr<BaseActionUndonable> & p_action );

		// inline void enqueue( BaseAction * const p_action ) { _actionQueue.emplace( p_action ); }
		bool canUndo() const;
		void undo();
		bool canRedo() const;
		void redo();

		void setBufferSize( const uint p_bufferSize );

		// virtual void update( const float & p_deltaTime ) override;

	  private:
		uint _bufferSize = 0;

		ListActionUndonablePtr _bufferUndo = ListActionUndonablePtr();
		ListActionUndonablePtr _bufferRedo = ListActionUndonablePtr();
		//  QueueVTXActionPtr	   _actionQueue = QueueVTXActionPtr();

		void _purgeBuffer();
	};
} // namespace VTX::App::Core::Action

namespace VTX::App
{
	inline Core::Action::ActionSystem & ACTION_SYSTEM() { return Util::Singleton<Core::Action::ActionSystem>::get(); }
} // namespace VTX::App
#endif
