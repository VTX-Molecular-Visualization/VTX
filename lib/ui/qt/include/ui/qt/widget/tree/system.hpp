#ifndef __VTX_UI_QT_WIDGET_TREE_SYSTEM__
#define __VTX_UI_QT_WIDGET_TREE_SYSTEM__

#include "ui/qt/widget/tree/base_tree.hpp"
#include <QContextMenuEvent>
#include <QTreeView>

namespace VTX::UI::QT::Widget::Tree
{
	/**
	 * @brief Scene tree.
	 */
	class System : public Widget::Tree::BaseTree<System, QTreeView>
	{
	  public:
		System( QWidget * );

		void contextMenuEvent( QContextMenuEvent * p_e ) override;
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
