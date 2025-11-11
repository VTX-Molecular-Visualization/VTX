#ifndef __VTX_UI_QT_WIDGET_TREE_VIEW__
#define __VTX_UI_QT_WIDGET_TREE_VIEW__

#include "ui/qt/menu/selection.hpp"
#include "ui/qt/model.hpp"
#include "ui/qt/services.hpp"
#include <QContextMenuEvent>
#include <QTreeView>

namespace VTX::UI::QT::Widget
{

	class Tree : public Widget::BaseWidget<Tree, QTreeView>
	{
	  public:
		Tree( QWidget * const p_parent = nullptr ) : BaseWidget( p_parent )
		{
			// UI/UX.
			setHeaderHidden( true );
			setItemsExpandable( true );
			setAllColumnsShowFocus( true );
			setUniformRowHeights( true );
			setExpandsOnDoubleClick( false );
			setEditTriggers( QAbstractItemView::NoEditTriggers );

			// Selection.
			setSelectionMode( QAbstractItemView::ExtendedSelection );
			setSelectionBehavior( QAbstractItemView::SelectRows );
		}

		void contextMenuEvent( QContextMenuEvent * p_e ) override
		{
			Menu::Selection menu( this );
			menu.exec( p_e->globalPos() );
		}
	};

} // namespace VTX::UI::QT::Widget

#endif
