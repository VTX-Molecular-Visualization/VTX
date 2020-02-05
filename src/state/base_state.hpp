#ifndef __VTX_BASE_STATE__
#define __VTX_BASE_STATE__

#ifdef _MSC_VER
#pragma once
#endif

#include "controller/base_controller.hpp"
#include "generic/base_collectionable.hpp"
#include "generic/base_updatable.hpp"
#include "id.hpp"

namespace VTX
{
	namespace State
	{
		class BaseState :
			public Generic::BaseUpdatable,
			public Generic::BaseCollectionable,
			public Generic::BaseEventHandler<SDL_Event>
		{
		  public:
			virtual ~BaseState() {}

			virtual void enter( void * const ) = 0;
			virtual void exit()				   = 0;

			virtual void BaseState::handleEvent( const SDL_Event & p_event, void * const p_arg ) override
			{
				if ( _controller != nullptr ) { _controller->handleEvent( p_event ); }
			}

		  protected:
			Controller::BaseController * _controller = nullptr;
		};
	} // namespace State
} // namespace VTX
#endif
