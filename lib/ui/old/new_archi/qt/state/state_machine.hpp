#ifndef __VTX_UI_QT_STATE_MACHINE__
#define __VTX_UI_QT_STATE_MACHINE__

#include "base_state.hpp"
#include <app/old/application/generic/base_updatable.hpp>
#include <app/old/id.hpp>
#include <map>
#include <memory>

namespace VTX::UI::QT::State
{
	class StateMachine : public App::Old::Application::Generic::BaseUpdatable
	{
	  public:
		StateMachine();
		~StateMachine();

		template<typename T, typename = std::enable_if<std::is_base_of<BaseState, T>::value>>
		inline T * const getState( const App::Old::VTX_ID & p_id )
		{
			return dynamic_cast<T * const>( _states[ p_id ] );
		}

		void goToState( const App::Old::VTX_ID &, void * const p_arg = nullptr );

		virtual void update( const float & ) override;

	  private:
		BaseState *									   _currentState = nullptr;
		std::map<const App::Old::VTX_ID, BaseState * const> _states		 = std::map<const App::Old::VTX_ID, BaseState * const>();

		void _switchState( BaseState * const, void * const p_arg );
	};
}; // namespace VTX::UI::QT::State
#endif
