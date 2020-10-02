#ifndef __VTX_UI_WIDGET_BASE_MENUTOOLBUTTON_WIDGET__
#define __VTX_UI_WIDGET_BASE_MENUTOOLBUTTON_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include <QToolButton>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class MenuToolButtonWidget : public BaseManualWidget<QToolButton>
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				void setData( const QString & p_name, const QString & p_iconUrl, const Qt::Orientation p_orientation );

				template<typename F>
				void setTriggerAction( const F * p_receiver, void ( F::*p_action )() )
				{
					p_receiver->connect( this, &QToolButton::clicked, p_receiver, p_action );
				}
				void localize() override;

			  protected:
				MenuToolButtonWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};
				void setupUi( const QString & p_name ) override;
				void setupSlots() override;

				void updateButtonStyle()
				{
					const Qt::Orientation currOrientation = width() > height() ? Qt::Orientation::Horizontal : Qt::Orientation::Vertical;
					updateButtonStyle( currOrientation );
				}
				virtual void updateButtonStyle( const Qt::Orientation p_orientation );
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
