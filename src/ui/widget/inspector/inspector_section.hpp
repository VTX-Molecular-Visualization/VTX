#ifndef __VTX_UI_WIDGET_INSPECTOR_SECTION__
#define __VTX_UI_WIDGET_INSPECTOR_SECTION__

#include "style.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget_factory.hpp"
#include <QGridLayout>
#include <QString>

namespace VTX::UI::Widget::Inspector
{
	class InspectorSection : public CustomWidget::CollapsingHeaderWidget
	{
		VTX_WIDGET

	  public:
		~InspectorSection() {};
		inline void localize() {};

	  protected:
		InspectorSection( QWidget * p_parent ) : CustomWidget::CollapsingHeaderWidget( p_parent ) {};
		void _setupUi( const QString & p_name ) override;
	};
	class InspectorSectionVLayout : public InspectorSection
	{
		VTX_WIDGET

	  public:
		void appendField( const std::string & p_label, QWidget * const p_widget );

	  protected:
		InspectorSectionVLayout( QWidget * p_parent ) : InspectorSection( p_parent ) {};
		void _setupUi( const QString & p_name ) override;

	  private:
		QGridLayout * _gridLayout;
	};

} // namespace VTX::UI::Widget::Inspector

#endif
