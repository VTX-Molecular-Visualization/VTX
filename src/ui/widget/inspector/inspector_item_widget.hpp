#ifndef __VTX_UI_WIDGET_INSPECTOR_ITEM__
#define __VTX_UI_WIDGET_INSPECTOR_ITEM__

#include "inspector_section_flag.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include <QWidget>

namespace VTX::UI::Widget::Inspector
{
	class InspectorItemWidget : public BaseManualWidget<QWidget>
	{
	  public:
		virtual void localize() override {};
		virtual void refresh( const SectionFlag & p_flag = SectionFlag::ALL ) {};

		void setExpanded( const bool p_expanded );

	  protected:
		InspectorItemWidget( QWidget * p_parent = nullptr ) : BaseManualWidget( p_parent ) {};

		virtual void _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override {};

		void								   _appendSection( InspectorSection * p_section );
		CustomWidget::CollapsingHeaderWidget * _getHeader() { return _mainWidget; }

	  private:
		CustomWidget::CollapsingHeaderWidget * _mainWidget	  = nullptr;
		QVBoxLayout *						   _contentLayout = nullptr;
	};

} // namespace VTX::UI::Widget::Inspector

#endif
