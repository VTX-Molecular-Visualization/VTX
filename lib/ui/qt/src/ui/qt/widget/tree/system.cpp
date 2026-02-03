#include "ui/qt/widget/tree/system.hpp"
#include "ui/qt/menu/selection.hpp"
#include "ui/qt/services.hpp"
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	System::System( QWidget * const p_parent ) : Widget::Tree::BaseTree<System, QTreeView>( p_parent )
	{
		// Selection.
		setSelectionBehavior( QAbstractItemView::SelectRows );

		// One expanded at a time.
		connect(
			this,
			&QTreeView::expanded,
			[ this ]( const QModelIndex & p_index )
			{
				QModelIndex parent	 = p_index.parent();
				const int	rowCount = model()->rowCount( parent );

				for ( int r = 0; r < rowCount; r++ )
				{
					QModelIndex sibling = model()->index( r, 0, parent );

					if ( sibling != p_index && isExpanded( sibling ) )
					{
						collapse( sibling );
					}
				}
			}
		);

		// Double click.
		connect(
			this,
			&QTreeView::doubleClicked,
			[ this ]( const QModelIndex & p_index ) { App::ACTION().execute<App::Action::Camera::Orient>(); }
		);
	}

	void System::contextMenuEvent( QContextMenuEvent * p_e )
	{
		Menu::Selection menu( this );
		menu.exec( p_e->globalPos() );
	}

} // namespace VTX::UI::QT::Widget::Tree
