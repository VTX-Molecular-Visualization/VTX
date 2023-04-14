#ifndef __VTX_BASE_CONTROLLER__
#define __VTX_BASE_CONTROLLER__

#include <util/types.hpp>
#include <app/old_app/generic/base_updatable.hpp>

namespace VTX
{
	namespace Controller
	{
		class BaseController : public Generic::BaseUpdatable
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
