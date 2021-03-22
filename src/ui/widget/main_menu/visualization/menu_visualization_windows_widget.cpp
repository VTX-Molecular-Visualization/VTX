#include "menu_visualization_windows_widget.hpp"
#include "id.hpp"
#include "ui/main_window.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Visualization
{
	MenuVisualizationWindowsWidget::MenuVisualizationWindowsWidget( QWidget * p_parent ) :
		MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE );
	}

	MenuVisualizationWindowsWidget::~MenuVisualizationWindowsWidget() {}

	void MenuVisualizationWindowsWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE )
		{
			refresh();
		}
	}

	void MenuVisualizationWindowsWidget::refresh()
	{
		const bool sequenceWindowVisible
			= VTXApp::get().getMainWindow().getWidgetVisibility( ID::UI::Window::SEQUENCE );

		if ( sequenceWindowVisible )
		{
			_sequence->setText( "Hide\nSequence" );
			_sequence->setIcon( Style::IconConst::get().HIDE_SEQUENCE_ICON );
		}
		else
		{
			_sequence->setText( "Show\nSequence" );
			_sequence->setIcon( Style::IconConst::get().SHOW_SEQUENCE_ICON );
		}
	}

	void MenuVisualizationWindowsWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		// Render view tools
		_sequence = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "toggleSequenceButton" );
		_sequence->setData( "Show\nSequence", ":/sprite/show_sequence_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_sequence, 0 );

		_infoUnderCursor
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "showInfoUnderCursorButton" );
		_infoUnderCursor->setData( "Show\nCursor Info", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_infoUnderCursor, 1 );

		_minimap = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "showMinimapButton" );
		_minimap->setData( "Show\nMinimap", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_minimap, 2 );

		validate();
	}
	void MenuVisualizationWindowsWidget::_setupSlots()
	{
		_sequence->setTriggerAction( this, &MenuVisualizationWindowsWidget::_openSequenceWindow );
	}
	void MenuVisualizationWindowsWidget::localize() { setTitle( "Windows" ); }

	void MenuVisualizationWindowsWidget::_openSequenceWindow() { VTXApp::get().getMainWindow().toggleSequenceWindow(); }
} // namespace VTX::UI::Widget::MainMenu::Visualization
