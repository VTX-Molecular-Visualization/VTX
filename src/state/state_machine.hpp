#ifndef __VTX_STATE_MACHINE__
#define __VTX_STATE_MACHINE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_state.hpp"
#include <magic_enum.hpp>
#include <map>
#include <memory>

namespace VTX
{
	namespace State
	{
		class StateMachine
		{
		  public:
			using StateSharedPtr		   = std::shared_ptr<BaseState>;
			using MapEnumToStateSharedPtr  = std::map<STATE_NAME, StateSharedPtr>;
			using PairEnumToStateSharedPtr = std::pair<STATE_NAME, StateSharedPtr>;

			StateMachine() = default;
			virtual ~StateMachine();

			virtual void init();
			virtual void update() final;
			void		 goToState( const STATE_NAME );

		  protected:
			virtual void _addState( const StateSharedPtr ) final;
			virtual void _addStates();

		  private:
			StateSharedPtr			_currentState = nullptr;
			MapEnumToStateSharedPtr _states		  = MapEnumToStateSharedPtr();

			void _switchState( const StateSharedPtr );
		};
	}; // namespace State
};	   // namespace VTX
#endif
