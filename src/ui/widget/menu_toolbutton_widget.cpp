#include "menu_toolbutton_widget.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
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
			void MenuToolButtonWidget::localize() {}
			void MenuToolButtonWidget::setupUi( const QString & p_name )
			{
				BaseManualWidget::setupUi( p_name );
				QSizePolicy sizePolicy = QSizePolicy( QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum );
				setSizePolicy( sizePolicy );
			}
			void MenuToolButtonWidget::setupSlots() {}

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

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
