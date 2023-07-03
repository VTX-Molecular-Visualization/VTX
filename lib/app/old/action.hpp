#ifndef __VTX_APP_OLD_ACTION__
#define __VTX_APP_OLD_ACTION__

#include "core/action/base_action.hpp"
#include "manager/action_manager.hpp"
// #include <memory>
#include <type_traits>

// TODO Apply VTX_ACTION<> whenever it's possible
namespace VTX
{
	inline void VTX_ACTION( App::Old::Core::Action::BaseAction * const p_action )
	{
		App::Old::Manager::ActionManager::get().execute( p_action );
	}

	inline void VTX_ACTION_ENQUEUE( App::Old::Core::Action::BaseAction * const p_action )
	{
		App::Old::Manager::ActionManager::get().enqueue( p_action );
	}

	// TODO manage unique_ptr here
	template<typename T, typename = std::enable_if<std::is_base_of<T, App::Old::Core::Action::BaseAction>::value>>
	inline void VTX_ACTION()
	{
		// const std::shared_ptr actionPtr = std::make_shared<T>();
		T * const actionPtr = new T();
		App::Old::Manager::ActionManager::get().execute( actionPtr );
	}
	template<typename T,
			 typename... Args,
			 typename = std::enable_if<std::is_base_of<T, App::Old::Core::Action::BaseAction>::value>>
	inline void VTX_ACTION( Args... _args )
	{
		// const std::shared_ptr actionPtr = std::make_shared<T>( _args... );
		T * const actionPtr = new T( _args... );
		App::Old::Manager::ActionManager::get().execute( actionPtr );
	}

	template<typename T, typename = std::enable_if<std::is_base_of<T, App::Old::Core::Action::BaseAction>::value>>
	inline void VTX_ACTION_ENQUEUE()
	{
		// const std::shared_ptr actionPtr = std::make_shared<T>();
		T * const actionPtr = new T();
		App::Old::Manager::ActionManager::get().enqueue( actionPtr );
	}
	template<typename T,
			 typename... Args,
			 typename = std::enable_if<std::is_base_of<T, App::Old::Core::Action::BaseAction>::value>>
	inline void VTX_ACTION_ENQUEUE( Args... _args )
	{
		// const std::shared_ptr actionPtr = std::make_shared<T>( _args... );
		T * const actionPtr = new T( _args... );
		App::Old::Manager::ActionManager::get().enqueue( actionPtr );
	}

	// TODO Create a command interpretor class which will convert the string into the action with the right parameters
	inline void VTX_ACTION( const std::string & p_action ) { App::Old::Manager::ActionManager::get().execute( p_action ); }
} // namespace VTX

#endif
