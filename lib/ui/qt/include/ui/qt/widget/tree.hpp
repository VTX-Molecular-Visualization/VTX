#ifndef __VTX_UI_QT_WIDGET_TREE_VIEW__
#define __VTX_UI_QT_WIDGET_TREE_VIEW__

#include "ui/qt/widget/base_widget.hpp"
#include <QContextMenuEvent>
#include <QTreeView>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Scene tree.
	 */
	class Tree : public Widget::BaseWidget<Tree, QTreeView>
	{
		Q_OBJECT

	  public:
		Tree( QWidget * );

		void contextMenuEvent( QContextMenuEvent * p_e ) override;
	};

} // namespace VTX::UI::QT::Widget

#endif
