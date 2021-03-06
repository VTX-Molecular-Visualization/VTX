#ifndef __VTX_UI_WIDGET_VDW_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_VDW_REPRESENTATION_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_representation_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/float_field_slider_widget.hpp"
#include <QLabel>
#include <QWidget>

namespace VTX::UI::Widget::Representation
{
	class VdwRepresentationWidget : public BaseRepresentationWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;
		void refresh() override;

	  protected:
		VdwRepresentationWidget( QWidget * p_parent = nullptr ) : BaseRepresentationWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		CustomWidget::FloatFieldSliderWidget * _sphereOffsetRadius = nullptr;
		QLabel *							   _label			   = nullptr;

		void _onRadiusOffsetChange( const float p_newRadiusOffset );
	};
} // namespace VTX::UI::Widget::Representation
#endif
