#ifndef __VTX_UI_QT_CONTROLLER_CONTROLLER_MANAGER__
#define __VTX_UI_QT_CONTROLLER_CONTROLLER_MANAGER__

#include "base_controller.hpp"
#include <util/collection.hpp>

namespace VTX::App::Core::Controller
{
	using ControllerCollection = Util::Collection<BaseController>;
} // namespace VTX::App::Core::Controller
#endif
