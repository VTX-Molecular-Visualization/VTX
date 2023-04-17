#include "ui/ui_generator.hpp"
#if VTX_UI_STYLE == VTX_UI_STYLE_QT
#include "ui/qt/application_qt.hpp"
#endif

namespace VTX::UI
{
	Core::BaseUIApplication * UIGenerator::createUI()
	{
		Core::BaseUIApplication * res;

#if VTX_UI_STYLE == VTX_UI_STYLE_COMMAND_LINE
		res = CommandLine::ApplicationCL();
#elif VTX_UI_STYLE == VTX_UI_STYLE_QT
		QT::ApplicationQt::configure();
		res = new QT::ApplicationQt();
#else
		res = nullptr;
#endif

		return res;
	}

} // namespace VTX::UI
