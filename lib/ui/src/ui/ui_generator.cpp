#include "ui/ui_generator.hpp"
#if VTX_UI_STYLE == VTX_UI_STYLE_QT
#include "ui/qt/application_qt.hpp"
#endif

namespace VTX::UI
{
	std::unique_ptr<Core::BaseUIApplication> UIGenerator::createUI()
	{
		std::unique_ptr<Core::BaseUIApplication> res;

#if VTX_UI_STYLE == VTX_UI_STYLE_COMMAND_LINE
		res = std::make_unique<CommandLine::ApplicationCL>();
#elif VTX_UI_STYLE == VTX_UI_STYLE_QT
		QT::ApplicationQt::configure();
		res = std::make_unique<QT::ApplicationQt>();
#else
		res = nullptr;
#endif

		return res;
	}

} // namespace VTX::UI
