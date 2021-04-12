#ifndef __VTX_UI_WIDGET_BALL_AND_STICK_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_BALL_AND_STICK_REPRESENTATION_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_representation_widget.hpp"

namespace VTX::UI::Widget::Representation
{
	class BallAndStickRepresentationWidget : public BaseRepresentationWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;
		void updateWithNewValue( const InstantiatedRepresentation & p_value ) override;

	  protected:
		BallAndStickRepresentationWidget( QWidget * p_parent = nullptr ) : BaseRepresentationWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;
		void _refresh() override;

		void _onSphereRadiusChange( const float p_newRadius ) override;
		void _onCylinderRadiusChange( const float p_newRadius ) override;
	};
} // namespace VTX::UI::Widget::Representation
#endif
