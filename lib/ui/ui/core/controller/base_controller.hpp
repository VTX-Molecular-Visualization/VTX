#ifndef __VTX_UI_CORE_CONTROLLER_BASE_CONTROLLER__
#define __VTX_UI_CORE_CONTROLLER_BASE_CONTROLLER__

#include <old/generic/base_updatable.hpp>

namespace VTX::UI::Core::Controller
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
} // namespace VTX::UI::Core::Controller
#endif
