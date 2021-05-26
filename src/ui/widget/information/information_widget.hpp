#ifndef __VTX_UI_WIDGET_INFORMATION__
#define __VTX_UI_WIDGET_INFORMATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/dock_window_main_widget.hpp"
#include <QDockWidget>
#include <QString>
#include <QWidget>

namespace VTX::UI::Widget::Information
{
	class InformationWidget : public BaseManualWidget<QDockWidget>
	{
		VTX_WIDGET

	  public:
		~InformationWidget() = default;

	  protected:
		InformationWidget( QWidget * );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void localize() override;

	  private:
		CustomWidget::DockWindowMainWidget<QWidget> * _mainWidget = nullptr;

		QString _getVersionText() const;
		QString _getLicenseText() const;
	};
} // namespace VTX::UI::Widget::Information
#endif
