#include "ui_generator.hpp"
#if VTX_UI_STYLE == VTX_UI_STYLE_QT
#include "qt/application.hpp"
#endif

namespace VTX::UI
{
	Core::Application * UIGenerator::createUI()
	{
		Core::Application * res;

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
