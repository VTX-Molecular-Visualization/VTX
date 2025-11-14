#include "ui/qt/widget/tree.hpp"
#include "ui/qt/menu/selection.hpp"
#include "ui/qt/model.hpp"
#include "ui/qt/services.hpp"

namespace VTX::UI::QT::Widget
{

	Tree::Tree( QWidget * const p_parent ) : BaseWidget( p_parent )
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

	void Tree::contextMenuEvent( QContextMenuEvent * p_e )
	{
		Menu::Selection menu( this );
		menu.exec( p_e->globalPos() );
	}

} // namespace VTX::UI::QT::Widget
