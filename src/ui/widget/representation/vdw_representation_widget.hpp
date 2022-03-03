#ifndef __VTX_UI_WIDGET_VDW_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_VDW_REPRESENTATION_WIDGET__

#include "base_representation_widget.hpp"

namespace VTX::UI::Widget::Representation
{
	class VdwRepresentationWidget : public BaseRepresentationWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;

	  protected:
		VdwRepresentationWidget( QWidget * p_parent = nullptr ) : BaseRepresentationWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;
	};
} // namespace VTX::UI::Widget::Representation
#endif
