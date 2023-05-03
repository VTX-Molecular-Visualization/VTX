#ifndef __VTX_BASE_CONTROLLER__
#define __VTX_BASE_CONTROLLER__

#include <app/application/generic/base_updatable.hpp>
#include <util/types.hpp>

namespace VTX
{
	namespace Controller
	{
		class BaseController : public App::Application::Generic::BaseUpdatable
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
