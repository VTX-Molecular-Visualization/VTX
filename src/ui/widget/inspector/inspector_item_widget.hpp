#ifndef __VTX_UI_WIDGET_INSPECTOR_MOLECULE__
#define __VTX_UI_WIDGET_INSPECTOR_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "inspector_section_flag.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::Inspector
{
	class InspectorItemWidget : public BaseManualWidget<QWidget>
	{
	  public:
		virtual void localize() override {};

	  protected:
		InspectorItemWidget( QWidget * p_parent = nullptr ) : BaseManualWidget( p_parent ) {};

		virtual void _setupUi( const QString & p_name ) override {};
		virtual void _setupSlots() override {};

		virtual void _refresh( const SectionFlag & _flag = SectionFlag::ALL ) {};
	};

} // namespace VTX::UI::Widget::Inspector

#endif
