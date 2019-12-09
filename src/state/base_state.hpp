#ifndef __VTX_BASE_STATE__
#define __VTX_BASE_STATE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../controller/base_controller.hpp"

namespace VTX
{
	namespace State
	{
		enum class STATE_NAME
		{
			LOADING,
			VISUALIZATION
		};

		class BaseState
		{
		  public:
			BaseState() = default;

			virtual STATE_NAME getName() const = 0;
			virtual void	   enter()		   = 0;
			virtual void	   exit()		   = 0;
			virtual void	   update() {}

			void handleEvent( const SDL_Event & );

		  protected:
			Controller::BaseController * _controller = nullptr;
		};
	} // namespace State
} // namespace VTX
#endif
