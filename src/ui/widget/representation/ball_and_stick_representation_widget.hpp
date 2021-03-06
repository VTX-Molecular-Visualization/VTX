#ifndef __VTX_UI_WIDGET_BALL_AND_STICK_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_BALL_AND_STICK_REPRESENTATION_WIDGET__

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
	class BallAndStickRepresentationWidget : public BaseRepresentationWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;
		void refresh() override;

	  protected:
		BallAndStickRepresentationWidget( QWidget * p_parent = nullptr ) : BaseRepresentationWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		QLabel *							   _ballLabel	= nullptr;
		QLabel *							   _stickLabel	= nullptr;
		CustomWidget::FloatFieldSliderWidget * _ballRadius	= nullptr;
		CustomWidget::FloatFieldSliderWidget * _stickRadius = nullptr;

		void _onBallRadiusChange( const float p_newRadius );
		void _onStickRadiusChange( const float p_newRadius );
	};
} // namespace VTX::UI::Widget::Representation
#endif
