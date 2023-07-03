#ifndef __VTX_UI_WIDGET_STICK_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_STICK_REPRESENTATION_WIDGET__

#include "base_representation_widget.hpp"

namespace VTX::UI::Widget::Representation
{
	class StickRepresentationWidget : public BaseRepresentationWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;

	  protected:
		StickRepresentationWidget( QWidget * p_parent = nullptr ) : BaseRepresentationWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;

		void _onCylinderRadiusChange( const float p_newRadius ) override;
	};
} // namespace VTX::UI::Widget::Representation
#endif
