#include "menu_toolbutton_widget.hpp"
#include <util/logger.hpp>
#include "qt/style.hpp"
#include <QKeyEvent>
#include <QLayout>
#include <QPalette>

namespace VTX::UI::QT::Widget::MainMenu
{
	MenuToolButtonWidget::MenuToolButtonWidget( QWidget * p_parent ) :
		UI::Core::MainMenu::MainMenuToolButton(), BaseManualWidget( p_parent ) {};

	void MenuToolButtonWidget::setName( const QString & p_name ) { setText( p_name ); }

	void MenuToolButtonWidget::setData( const QString &		  p_name,
										const QString &		  p_iconUrl,
										const Qt::Orientation p_orientation )
	{
		const QIcon & icon = p_iconUrl.isEmpty() ? QIcon() : QIcon::fromTheme( p_iconUrl );
		setData( p_name, icon, p_orientation );
	}
	void MenuToolButtonWidget::setData( const QString &		  p_name,
										const QIcon &		  p_icon,
										const Qt::Orientation p_orientation )
	{
		if ( !p_name.isEmpty() )
			setText( p_name );

		if ( !p_icon.isNull() )
			setIcon( p_icon );

		_updateButtonStyle( p_orientation );
	}

	void MenuToolButtonWidget::setOrientation( const Qt::Orientation p_orientation )
	{
		_updateButtonStyle( p_orientation );
	}

	void MenuToolButtonWidget::showActiveFeedback( const bool p_activate )
	{
		UI::Core::MainMenu::MainMenuToolButton::showActiveFeedback( p_activate );

		if ( _hasActiveFeedback )
		{
			QPalette highlightedPalette = palette();
			highlightedPalette.setColor( QPalette::ColorRole::Button, Style::HIGHLIGHTED_COLOR );
			setPalette( highlightedPalette );
		}
		else
		{
			QPalette highlightedPalette = palette();
			highlightedPalette.setColor( QPalette::ColorRole::Button, Style::BACKGROUND_COLOR );
			setPalette( highlightedPalette );
		}
	}

	void MenuToolButtonWidget::localize() {}
	void MenuToolButtonWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setSizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding );
		setFont( Style::TOOL_MENU_BUTTON_FONT() );
		setPopupMode( QToolButton::ToolButtonPopupMode::InstantPopup );
	}
	void MenuToolButtonWidget::_setupSlots() {}

	void MenuToolButtonWidget::_updateButtonStyle( const Qt::Orientation p_orientation )
	{
		switch ( p_orientation )
		{
		case Qt::Orientation::Horizontal:
			setToolButtonStyle( Qt::ToolButtonStyle::ToolButtonTextBesideIcon );
			setContentsMargins( 2, 2, 2, 2 );
			break;
		case Qt::Orientation::Vertical:
			setToolButtonStyle( Qt::ToolButtonStyle::ToolButtonTextUnderIcon );
			setContentsMargins( 10, 2, 10, 2 );
			break;
		default:
			VTX_WARNING( "Orientation " + ( std::to_string( p_orientation ) )
						 + " not managed in BaseMenuToolButtonWidget::updateButtonStyle. Set Default" );
			_updateButtonStyle( Qt::Orientation::Vertical );
			break;
		}
	}

	void MenuToolButtonWidget::keyPressEvent( QKeyEvent * p_event )
	{
		if ( !arrowNavigation.keyPressed( p_event ) )
			BaseManualWidget::keyPressEvent( p_event );
	}

} // namespace VTX::UI::QT::Widget::MainMenu
