#ifndef __VTX_UI_WIDGET_SELECTION__
#define __VTX_UI_WIDGET_SELECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "selection/selection_enum.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QComboBox>
#include <QDockWidget>
#include <QString>
#include <QTreeWidgetItem>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Selection
			{
				class SelectionWidget : public BaseManualWidget<QDockWidget>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void receiveEvent( const Event::VTXEvent & p_event ) override;
					void localize() override;

				  protected:
					SelectionWidget( QWidget * p_parent );
					void _setupUi( const QString & p_name ) override;
					void _setupSlots() override;

				  private:
					QTreeWidget * _treeWidget			 = nullptr;
					QComboBox *	  _selectionTypeComboBox = nullptr;

					void _populateItemList();
					void _selectionTypeChangedAction( const int p_newIndex );
				};
			} // namespace Selection
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
#endif
