#ifndef __VTX_APP_ACTION__
#define __VTX_APP_ACTION__

#include "manager/action_manager.hpp"

// TODO Apply VTX_ACTION<> whenever it's possible
namespace VTX
{
	inline void VTX_ACTION( App::Core::Action::BaseAction * const p_action )
	{
		App::Manager::ActionManager::get().execute( p_action );
	}

	inline void VTX_ACTION_ENQUEUE( App::Core::Action::BaseAction * const p_action )
	{
		App::Manager::ActionManager::get().enqueue( p_action );
	}

	// TODO manage unique_ptr here
	template<typename T>
	inline void VTX_ACTION()
	{
		// const std::shared_ptr actionPtr = std::make_shared<T>();
		T * const actionPtr = new T();
		App::Manager::ActionManager::get().execute( actionPtr );
	}
	template<typename T, typename... Args>
	inline void VTX_ACTION( Args... _args )
	{
		// const std::shared_ptr actionPtr = std::make_shared<T>( _args... );
		T * const actionPtr = new T( _args... );
		App::Manager::ActionManager::get().execute( actionPtr );
	}

	template<typename T>
	inline void VTX_ACTION_ENQUEUE()
	{
		// const std::shared_ptr actionPtr = std::make_shared<T>();
		T * const actionPtr = new T();
		App::Manager::ActionManager::get().enqueue( actionPtr );
	}
	template<typename T, typename... Args>
	inline void VTX_ACTION_ENQUEUE( Args... _args )
	{
		// const std::shared_ptr actionPtr = std::make_shared<T>( _args... );
		T * const actionPtr = new T( _args... );
		App::Manager::ActionManager::get().enqueue( actionPtr );
	}

	inline void VTX_ACTION( const std::string & p_action ) { App::Manager::ActionManager::get().execute( p_action ); }
} // namespace VTX

#endif
