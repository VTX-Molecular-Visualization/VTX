#ifndef __VTX_MACHINE__
#define __VTX_MACHINE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../generic/base_updatable.hpp"
#include "../generic/has_collection.hpp"
#include "base_state.hpp"
#include <map>
#include <memory>

namespace VTX
{
	namespace State
	{
		class Machine :
			public Generic::BaseUpdatable,
			public Generic::HasCollection<BaseState>,
			public Generic::BaseEventHandler<SDL_Event>
		{
		  public:
			void goToState( const std::string &, void * const p_arg = nullptr );

			virtual void handleEvent( const SDL_Event &, void * const = nullptr ) override;
			virtual void update( const double ) override;

		  protected:
			virtual void _addItems() override;

		  private:
			BaseState * _currentState = nullptr;

			void _switchState( BaseState * const, void * const p_arg );
		};
	}; // namespace State
};	   // namespace VTX
#endif
