#ifndef __VTX_UI_WIDGET_BALL_STICK_AND_CARTOON_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_BALL_STICK_AND_CARTOON_REPRESENTATION_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_representation_widget.hpp"

namespace VTX::UI::Widget::Representation
{
	class BallStickAndCartoonRepresentationWidget : public BaseRepresentationWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;
		void _refresh() override;

	  protected:
		BallStickAndCartoonRepresentationWidget( QWidget * p_parent = nullptr ) :
			BaseRepresentationWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;
	};
} // namespace VTX::UI::Widget::Representation
#endif
