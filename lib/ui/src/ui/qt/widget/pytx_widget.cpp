#include "ui/qt/widget/pytx_widget.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/widget/pytx/include_python_binding.hpp"

namespace VTX::UI::QT::Widget
{
	PyTXWidget::PyTXWidget()
	{
		PythonBinding::INTERPRETOR().addBinder<VTX::PythonBinding::Binding::VTXAppBinder>();
		QT_APP()->onInitEnded += []() { PythonBinding::INTERPRETOR().init(); };
	}

} // namespace VTX::UI::QT::Widget
