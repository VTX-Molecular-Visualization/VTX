#ifndef __VTX_APP_CORE_CONTROLLER_BASE_CONTROLLER__
#define __VTX_APP_CORE_CONTROLLER_BASE_CONTROLLER__

#include <util/generic/base_nameable.hpp>

namespace VTX::App::Core::Controller
{

	class BaseController : public Util::Generic::BaseNameable
	{
	  public:
		virtual ~BaseController() = default;
	};

} // namespace VTX::App::Core::Controller

#endif
