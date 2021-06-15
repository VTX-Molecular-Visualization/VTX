#ifndef __VTX_UI_WIDGET_CARTOON_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_CARTOON_REPRESENTATION_WIDGET__

#include "base_representation_widget.hpp"

namespace VTX::UI::Widget::Representation
{
	class CartoonRepresentationWidget : public BaseRepresentationWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;
		void updateWithNewValue( const InstantiatedRepresentation & p_value ) override;

	  protected:
		CartoonRepresentationWidget( QWidget * p_parent = nullptr ) : BaseRepresentationWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;
		void _refresh() override;
	};
} // namespace VTX::UI::Widget::Representation
#endif
