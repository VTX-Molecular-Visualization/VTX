#ifndef __VTX_UI_QT_WIDGET_TREE_VIEW__
#define __VTX_UI_QT_WIDGET_TREE_VIEW__

#include "ui/qt/model.hpp"
#include <QTreeView>

namespace VTX::UI::QT::Widget
{

	class Tree : public Widget::BaseWidget<Tree, QTreeView>
	{
	  public:
		Tree( QWidget * const p_parent = nullptr ) : BaseWidget( p_parent )
		{
			setHeaderHidden( true );
			setRootIsDecorated( false );
			setItemsExpandable( false );
			setAllColumnsShowFocus( true );
			setSelectionMode( QAbstractItemView::SingleSelection );
			setEditTriggers( QAbstractItemView::NoEditTriggers );
			setUniformRowHeights( true );
			setExpandsOnDoubleClick( false );
			// Model
			//_model = new Model::Tree( this );
			// QTreeView::setModel( _model );
			// connect( this, &QTreeView::clicked, this, &Tree::_onItemClicked );
		}
	};

} // namespace VTX::UI::QT::Widget

#endif
