#include "menu_toolbutton_widget.hpp"
#include "style.hpp"
#include "vtx_app.hpp"
#include <QPalette>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				void MenuToolButtonWidget::setData( const QString & p_name, const QString & p_iconUrl, const Qt::Orientation p_orientation )
				{
					if ( !p_name.isEmpty() )
						setText( p_name );

					if ( !p_iconUrl.isEmpty() )
					{
						QIcon icon = QIcon::fromTheme( p_iconUrl );
						if ( icon.isNull() )
							VTX_WARNING( "icon " + p_iconUrl.toStdString() + " not found." );
						else
							setIcon( icon );
					}

					updateButtonStyle( p_orientation );
				}

				void MenuToolButtonWidget::setOrientation( const Qt::Orientation p_orientation ) { updateButtonStyle( p_orientation ); }

				void MenuToolButtonWidget::showActiveFeedback( const bool p_activate )
				{
					_hasActiveFeedback = p_activate;

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
					setSizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Minimum );
					setFont( Style::TOOL_MENU_BUTTON_FONT );
				}
				void MenuToolButtonWidget::_setupSlots() {}

				void MenuToolButtonWidget::updateButtonStyle( const Qt::Orientation p_orientation )
				{
					switch ( p_orientation )
					{
					case Qt::Orientation::Horizontal:
						setToolButtonStyle( Qt::ToolButtonStyle::ToolButtonTextBesideIcon );
						setIconSize( QSize( 16, 16 ) );
						break;
					case Qt::Orientation::Vertical:
						setToolButtonStyle( Qt::ToolButtonStyle::ToolButtonTextUnderIcon );
						setIconSize( QSize( 24, 24 ) );
						break;
					default:
						VTX_WARNING( "Orientation " + ( std::to_string( p_orientation ) ) + " not managed in BaseMenuToolButtonWidget::updateButtonStyle. Set Default" );
						updateButtonStyle( Qt::Orientation::Vertical );
						break;
					}
				}

			} // namespace MainMenu
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
