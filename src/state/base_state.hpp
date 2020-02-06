#ifndef __VTX_BASE_STATE__
#define __VTX_BASE_STATE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../generic/has_collection.hpp"
#include "controller/base_controller.hpp"
#include "generic/base_collectionable.hpp"
#include "generic/base_updatable.hpp"
#include "id.hpp"
#include <vector>

namespace VTX
{
	namespace State
	{
		class BaseState :
			public Generic::HasCollection<Controller::BaseController>,
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
				for ( PairStringToItemPtr controller : _getItems() )
				{
					controller.second->handleEvent( p_event );
				}
			}

			virtual void BaseState::update( const double p_deltaTime ) override
			{
				for ( PairStringToItemPtr controller : _getItems() )
				{
					controller.second->update( p_deltaTime );
				}
			}
		};
	} // namespace State
} // namespace VTX
#endif
