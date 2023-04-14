#ifndef __VTX_UI_WIDGET_SES_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_SES_REPRESENTATION_WIDGET__

#include "base_representation_widget.hpp"
#include "old_ui/ui/widget/base_manual_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::Representation
{
	class SesRepresentationWidget : public BaseRepresentationWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;

	  protected:
		SesRepresentationWidget( QWidget * p_parent = nullptr ) : BaseRepresentationWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;
	};
} // namespace VTX::UI::Widget::Representation
#endif
