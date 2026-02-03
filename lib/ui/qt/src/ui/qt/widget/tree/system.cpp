#include "ui/qt/widget/tree/system.hpp"
#include "ui/qt/delegate/scene_item_delegate.hpp"
#include "ui/qt/menu/selection.hpp"
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	System::System( const App::ECS::Entity p_system, QWidget * p_parent ) :
		_system( p_system ), Widget::Tree::BaseTree<System, QTreeView>( p_parent )
	{
		setExpandsOnDoubleClick( false );

		// Model.
		setModel( new SystemModel( p_system, this ) );

		// Selection.
		setSelectionModel( new SystemSelectionModel( p_system, model(), this ) );
		setSelectionBehavior( QAbstractItemView::SelectRows );

		// Delegate.
		setItemDelegate( new Delegate::SceneItemDelegate( p_system, this ) );

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
