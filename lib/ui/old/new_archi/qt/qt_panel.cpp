#include "ui/qt/qt_panel.hpp"
#include <app/old/core/event/vtx_event.hpp>
#include <app/old/event.hpp>
#include <app/old/event/global.hpp>

namespace VTX::UI::QT
{
	QtPanel::QtPanel() {}
	QtPanel::~QtPanel() {}

	void QtPanel::setVisible( const bool p_visible )
	{
		_changeVisibility( p_visible );
		VTX_EVENT( VTX::App::Old::Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE );
	}

	QtDockablePanel::QtDockablePanel( QWidget * const p_parent ) : QtPanelTemplate() {}
	QtDockablePanel::~QtDockablePanel() {}

	QtFloatingWindowPanel::QtFloatingWindowPanel( QWidget * const p_parent ) : QtPanelTemplate() {}
	QtFloatingWindowPanel::~QtFloatingWindowPanel() {}
} // namespace VTX::UI::QT
