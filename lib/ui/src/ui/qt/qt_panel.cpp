#include "ui/qt/qt_panel.hpp"
#include "ui/qt/widget/custom/dock_window_main_widget.hpp"

namespace VTX::UI::QT
{
	QtPanel::QtPanel() {}
	QtPanel::~QtPanel() {}

	void QtPanel::setVisible( const bool p_visible ) { _changeVisibility( p_visible ); }

	QtDockablePanel::QtDockablePanel( QWidget * const p_parent ) : QtPanelTemplate() {}
	QtDockablePanel::~QtDockablePanel() {}

	QWidget * QtDockablePanel::_instantiateMainWidget( const QSize & p_preferredSize, const QSize & p_minimalSize )
	{
		Widget::Custom::DockWindowMainWidget * mainWidget
			= new Widget::Custom::DockWindowMainWidget( p_preferredSize, p_minimalSize, this );

		return mainWidget;
	}

	QtFloatingWindowPanel::QtFloatingWindowPanel( QWidget * const p_parent ) : QtPanelTemplate() {}
	QtFloatingWindowPanel::~QtFloatingWindowPanel() {}
} // namespace VTX::UI::QT
