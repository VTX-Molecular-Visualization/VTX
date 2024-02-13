#ifndef __VTX_UI_QT_CONTROLLER_CONTROLLER_MANAGER__
#define __VTX_UI_QT_CONTROLLER_CONTROLLER_MANAGER__

#include "ui/qt/controller/base_controller.hpp"
#include <app/core/collection.hpp>

namespace VTX::UI::QT::Controller
{
	using ControllerCollection = App::Core::Collection<BaseController>;
} // namespace VTX::UI::QT::Controller
#endif
