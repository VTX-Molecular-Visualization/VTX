#ifndef __VTX_UI_WIDGET_STICK_AND_CARTOON_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_STICK_AND_CARTOON_REPRESENTATION_WIDGET__

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
	class StickAndCartoonRepresentationWidget : public BaseRepresentationWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;
		void refresh() override;

	  protected:
		StickAndCartoonRepresentationWidget( QWidget * p_parent = nullptr ) : BaseRepresentationWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QLabel *							   _label		= nullptr;
		CustomWidget::FloatFieldSliderWidget * _stickRadius = nullptr;

		void _onRadiusChange( const float p_newRadius );
	};
} // namespace VTX::UI::Widget::Representation
#endif
