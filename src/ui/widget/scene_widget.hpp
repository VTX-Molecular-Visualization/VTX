#ifndef __VTX_UI_WIDGET_SCENE__
#define __VTX_UI_WIDGET_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include "selection/base_selectable.hpp"
#include <QDockWidget>
#include <QTreeWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class SceneWidget : public BaseManualWidget<QDockWidget>
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				void receiveEvent( const Event::VTXEvent & p_event ) override;
				void localize() override;

				void			  addItem( QTreeWidgetItem * p_item );
				void			  addItem( QTreeWidgetItem * p_item, QTreeWidgetItem * p_parent );
				QTreeWidgetItem * takeItem( QTreeWidgetItem * p_item );
				void			  deleteItem( QTreeWidgetItem * p_item );

			  protected:
				SceneWidget( QWidget * p_parent );
				void _setupUi( const QString & p_name ) override;
				void _setupSlots() override;
				Selection::BaseSelectable * _getSelectableFromTreeWidgetItem( QTreeWidgetItem * treeWidgetItem ) const;

			  private:
				QTreeWidget * _treeWidget;

				void _onSelectionChange();
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
