#ifndef __VTX_UI_QT_CONTROLLER_CONTROLLER_MANAGER__
#define __VTX_UI_QT_CONTROLLER_CONTROLLER_MANAGER__

#include "base_controller.hpp"
#include <app/core/collection.hpp>

namespace VTX::App::Core::Controller
{
	using ControllerCollection = App::Core::Collection<BaseController>;
} // namespace VTX::App::Core::Controller
#endif
