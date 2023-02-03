#include "qt_panel.hpp"

namespace VTX::UI::QT
{
	QtPanel::QtPanel() {}
	QtPanel::~QtPanel() {}

	QtDockablePanel::QtDockablePanel() : QtPanel(), Widget::BaseManualWidget<QDockWidget>() {}
	QtDockablePanel::~QtDockablePanel() {}

	QtFloatingWindowPanel::QtFloatingWindowPanel() : QtPanel() {}
	QtFloatingWindowPanel::~QtFloatingWindowPanel() {}
} // namespace VTX::UI::QT
