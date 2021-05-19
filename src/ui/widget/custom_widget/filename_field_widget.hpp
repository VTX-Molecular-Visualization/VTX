#ifndef __VTX_UI_WIDGET_CUSTOM_FILENAME_FIELD__
#define __VTX_UI_WIDGET_CUSTOM_FILENAME_FIELD__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/base_manual_widget.hpp"
#include <QLineEdit>
#include <QString>
#include <QWidget>

namespace VTX::UI::Widget::CustomWidget
{
	class FilenameFieldWidget : public BaseManualWidget<QLineEdit>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		void localize() override;

	  protected:
		FilenameFieldWidget( QWidget * p_parent = nullptr );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
	};
} // namespace VTX::UI::Widget::CustomWidget
#endif
