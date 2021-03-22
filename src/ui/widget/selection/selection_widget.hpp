#ifndef __VTX_UI_WIDGET_SELECTION__
#define __VTX_UI_WIDGET_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "selection/selection_enum.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/dock_window_main_widget.hpp"
#include <QComboBox>
#include <QDockWidget>
#include <QString>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

namespace VTX::UI::Widget::Selection
{
	class SelectionWidget : public BaseManualWidget<QDockWidget>
	{
		VTX_WIDGET

	  public:
		void receiveEvent( const Event::VTXEvent & p_event ) override;
		void localize() override;

	  protected:
		SelectionWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		CustomWidget::DockWindowMainWidget<QWidget> * _mainWidget			 = nullptr;
		QVBoxLayout *								  _layout				 = nullptr;
		QComboBox *									  _selectionTypeComboBox = nullptr;

		void _populateItemList();
		void _selectionTypeCurrentIndexChanged( const int p_newIndex );
	};
} // namespace VTX::UI::Widget::Selection
#endif
