#ifndef __VTX_UI_WIDGET_SELECTION__
#define __VTX_UI_WIDGET_SELECTION__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/dock_window_main_widget.hpp"
#include <QComboBox>
#include <QDockWidget>
#include <QString>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <app/model/selection.hpp>
#include <app/old_app/selection/selection_enum.hpp>

namespace VTX::UI::Widget::Selection
{
	class SelectionWidget : public BaseManualWidget<QDockWidget>
	{
		VTX_WIDGET

	  public:
		void receiveEvent( const VTX::Event::VTXEvent & p_event ) override;
		void localize() override;

	  protected:
		SelectionWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		CustomWidget::DockWindowMainWidget<QWidget> * _mainWidget			 = nullptr;
		QVBoxLayout *								  _layout				 = nullptr;
		QComboBox *									  _selectionTypeComboBox = nullptr;

		void _addSelectionModel( Model::Selection * const p_selection );

		void _populateItemList();
		void _selectionTypeCurrentIndexChanged( const int p_newIndex );
	};
} // namespace VTX::UI::Widget::Selection
#endif
