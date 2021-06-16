#ifndef __VTX_UI_WIDGET_BALL_STICK_AND_CARTOON_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_BALL_STICK_AND_CARTOON_REPRESENTATION_WIDGET__

#include "base_representation_widget.hpp"

namespace VTX::UI::Widget::Representation
{
	class BallStickAndCartoonRepresentationWidget : public BaseRepresentationWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;
		void updateWithNewValue( const InstantiatedRepresentation & p_value ) override;

	  protected:
		BallStickAndCartoonRepresentationWidget( QWidget * p_parent = nullptr ) :
			BaseRepresentationWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;
		void _refresh() override;

		void _onSphereRadiusChange( const float p_newRadius ) override;
		void _onCylinderRadiusChange( const float p_newRadius ) override;
	};
} // namespace VTX::UI::Widget::Representation
#endif
