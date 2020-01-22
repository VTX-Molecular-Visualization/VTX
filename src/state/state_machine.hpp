#ifndef __VTX_STATE_MACHINE__
#define __VTX_STATE_MACHINE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../generic/base_updatable.hpp"
#include "base_state.hpp"
#include <map>
#include <memory>

namespace VTX
{
	namespace State
	{
		class StateMachine : public Generic::BaseUpdatable, public Generic::BaseEventHandler<SDL_Event>
		{
		  public:
			using StateSharedPtr		   = std::shared_ptr<BaseState>;
			using MapEnumToStateSharedPtr  = std::map<STATE_NAME, StateSharedPtr>;
			using PairEnumToStateSharedPtr = std::pair<STATE_NAME, StateSharedPtr>;

			StateMachine() = default;
			virtual ~StateMachine();

			virtual void init();
			void		 goToState( const STATE_NAME, void * const p_arg = nullptr );

			virtual void handleEvent( const SDL_Event &, void * const = nullptr ) override;
			virtual void update( const double ) override;

		  protected:
			virtual void _addState( const StateSharedPtr ) final;
			virtual void _addStates();

		  private:
			StateSharedPtr			_currentState = nullptr;
			MapEnumToStateSharedPtr _states		  = MapEnumToStateSharedPtr();

			void _switchState( const StateSharedPtr, void * const p_arg );
		};
	}; // namespace State
};	   // namespace VTX
#endif
