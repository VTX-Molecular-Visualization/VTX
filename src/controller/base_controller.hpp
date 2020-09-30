#ifndef __VTX_BASE_CONTROLLER__
#define __VTX_BASE_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/base_updatable.hpp"

namespace VTX
{
	namespace Controller
	{
		class BaseController : public Generic::BaseUpdatable//, public Event::BaseEventReceiverSDL
		{
		  public:
			BaseController()		  = default;
			virtual ~BaseController() = default;

			bool		 isActive() const { return _isActive; }
			virtual void setActive( const bool p_active ) { _isActive = p_active; }

		  protected:
			bool _isActive = true;
		};
	} // namespace Controller
} // namespace VTX
#endif
