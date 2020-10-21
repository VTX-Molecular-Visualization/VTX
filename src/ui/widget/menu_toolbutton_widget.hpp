#ifndef __VTX_UI_WIDGET_BASE_MENUTOOLBUTTON_WIDGET__
#define __VTX_UI_WIDGET_BASE_MENUTOOLBUTTON_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include <QToolButton>
#include <QWidget>
#include <type_traits>

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
				void setOrientation( const Qt::Orientation p_orientation );

				template<typename F, typename = std::enable_if<std::is_base_of<QWidget, F>::value>>
				void setTriggerAction( const F * const p_receiver, void ( F::*p_action )() )
				{
					p_receiver->connect( this, &QToolButton::clicked, p_receiver, p_action );
				}
				void localize() override;

			  protected:
				MenuToolButtonWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};
				void _setupUi( const QString & p_name ) override;
				void _setupSlots() override;

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
