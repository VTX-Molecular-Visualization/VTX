#ifndef __VTX_UI_WIDGET_BASE_MENUTOOLBUTTON_WIDGET__
#define __VTX_UI_WIDGET_BASE_MENUTOOLBUTTON_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_widget.hpp"
#include "vtx_app.hpp"
#include <QResizeEvent>
#include <QToolButton>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			template<typename T>
			class BaseMenuToolButtonWidget : public BaseWidget<QToolButton, T>
			{
			  public:
				BaseMenuToolButtonWidget( QWidget * p_parent ) : BaseWidget( p_parent ) { updateButtonStyle(); };

			  protected:
				virtual void resizeEvent( QResizeEvent * event ) override
				{
					QToolButton::resizeEvent( event );

					const QSize previousSize = event->oldSize();
					const QSize currentSize	 = event->size();

					const bool wasHorizontal = previousSize.width() > previousSize.height();
					const bool isHorizontal	 = currentSize.width() > currentSize.height();

					if ( wasHorizontal != isHorizontal )
					{
						if ( isHorizontal )
							updateButtonStyle( Qt::Orientation::Horizontal );
						else
							updateButtonStyle( Qt::Orientation::Vertical );
					}
				};

				void updateButtonStyle()
				{
					const Qt::Orientation currOrientation = width() > height() ? Qt::Orientation::Horizontal : Qt::Orientation::Vertical;
					updateButtonStyle( currOrientation );
				}

				virtual void updateButtonStyle( const Qt::Orientation orientation )
				{
					/*switch ( orientation )
					{
					case Qt::Orientation::Horizontal: setToolButtonStyle( Qt::ToolButtonStyle::ToolButtonTextBesideIcon ); break;
					case Qt::Orientation::Vertical: setToolButtonStyle( Qt::ToolButtonStyle::ToolButtonTextUnderIcon ); break;
					default:
						VTX_WARNING( "Orientation " + ( std::to_string( orientation ) ) + " not managed in BaseMenuToolButtonWidget::updateButtonStyle. Set Default" );
						updateButtonStyle( Qt::Orientation::Vertical );
						break;
					}*/
				}
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
