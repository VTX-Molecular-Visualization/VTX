#ifndef __VTX_UI_QT_WIDGET_TREE_VIEW__
#define __VTX_UI_QT_WIDGET_TREE_VIEW__

#include "ui/qt/model.hpp"
#include "ui/qt/services.hpp"
#include <QTreeView>

namespace VTX::UI::QT::Widget
{

	class Tree : public Widget::BaseWidget<Tree, QTreeView>
	{
	  public:
		Tree( QWidget * const p_parent = nullptr ) : BaseWidget( p_parent )
		{
			setHeaderHidden( true );
			// setRootIsDecorated( true );
			setItemsExpandable( true );
			setAllColumnsShowFocus( true );
			setUniformRowHeights( true );

			setExpandsOnDoubleClick( false );

			setEditTriggers( QAbstractItemView::NoEditTriggers );

			setSelectionMode( QAbstractItemView::ExtendedSelection );
			setSelectionBehavior( QAbstractItemView::SelectRows );
		}
	};

} // namespace VTX::UI::QT::Widget

#endif
