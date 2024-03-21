#include "ui/qt/tool/pytx_tool.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/tool/pytx/details/include_python_binding.hpp"

namespace VTX::UI::QT::Tool
{
	PyTXTool::PyTXTool() : BaseQtTool() {}

	void PyTXTool::instantiateTool()
	{
		PythonBinding::INTERPRETOR().addBinder<VTX::PythonBinding::Binding::VTXAppBinder>();
		QT_APP()->onInitEnded += []() { PythonBinding::INTERPRETOR().init(); };
	}

} // namespace VTX::UI::QT::Tool
