#ifndef __VTX_BASE_STATE__
#define __VTX_BASE_STATE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../controller/base_controller.hpp"
#include "../generic/base_updatable.hpp"

namespace VTX
{
	namespace State
	{
		enum class STATE_NAME
		{
			LOADING,
			VISUALIZATION
		};

		class BaseState : public Generic::BaseUpdatable, public Generic::BaseEventHandler<SDL_Event>
		{
		  public:
			BaseState() = default;

			virtual STATE_NAME getStateName() const	 = 0;
			virtual void	   enter( void * const ) = 0;
			virtual void	   exit()				 = 0;

			virtual void handleEvent( const SDL_Event &, void * const = nullptr ) override;

		  protected:
			Controller::BaseController * _controller = nullptr;
		};
	} // namespace State
} // namespace VTX
#endif
