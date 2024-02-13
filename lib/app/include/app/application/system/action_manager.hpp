#ifndef __VTX_APP_APPLICATION_ACTION_ACTION_MANAGER__
#define __VTX_APP_APPLICATION_ACTION_ACTION_MANAGER__

#include "app/application/system/system_registration.hpp"
#include "app/core/action/base_action.hpp"
#include "app/core/action/base_action_undonable.hpp"
#include "app/core/system/base_system.hpp"
#include <concepts>
#include <list>
#include <memory>
#include <util/concepts.hpp>
#include <util/types.hpp>

namespace VTX::App::Application::System
{
	template<typename T>
	concept UndonableActionConcept = std::derived_from<T, Core::Action::BaseActionUndonable>;
	template<typename T>
	concept ActionConcept = !
	UndonableActionConcept<T> && std::derived_from<T, Core::Action::BaseAction>;

	class ActionManager : public Core::System::BaseSystem
	{
	  public:
		inline static const System::SystemRegistration<ActionManager> SYSTEM
			= System::SystemRegistration<ActionManager>();

	  public:
		using BaseAction		  = Core::Action::BaseAction;
		using BaseActionUndonable = Core::Action::BaseActionUndonable;

		using ListActionUndonablePtr = std::list<std::unique_ptr<BaseActionUndonable>>;
		// using QueueVTXActionPtr		 = std::queue<BaseAction *>;

	  public:
		ActionManager();
		~ActionManager();

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
} // namespace VTX::App::Application::System

namespace VTX::App
{
	Application::System::ActionManager & VTX_ACTION();
}
#endif
