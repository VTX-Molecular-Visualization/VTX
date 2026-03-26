#include "ui/qt/widget/tree/system.hpp"
#include "ui/qt/delegate/system_delegate.hpp"
#include "ui/qt/menu/color_scheme.hpp"
#include "ui/qt/menu/representation.hpp"
#include "ui/qt/menu/selection.hpp"
#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>
#include <app/action/color.hpp>
#include <app/action/representation.hpp>
#include <app/action/selection.hpp>
#include <app/action/visibility.hpp>
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <app/system/trajectory.hpp>

namespace VTX::UI::QT::Widget::Tree
{
	namespace
	{
		QModelIndex _stepToward( QTreeView & p_view, const QModelIndex & p_from, const QModelIndex & p_to )
		{
			QModelIndex next = p_view.indexBelow( p_from );
			while ( next.isValid() )
			{
				if ( next == p_to )
				{
					return p_view.indexBelow( p_from );
				}
				next = p_view.indexBelow( next );
			}

			return p_view.indexAbove( p_from );
		}
	} // namespace

	System::System( const App::ECS::Entity p_system, QWidget * p_parent ) :
		Widget::Tree::BaseTree<System, QTreeView>( p_parent ), _system( p_system )
	{
		setExpandsOnDoubleClick( false );
		setMouseTracking( true );
		setIndentation( 12 );

		// Model.
		setModel( new Model::SystemModel( p_system, this ) );

		// Selection is rendered from app state, not from Qt selection.
		setSelectionMode( QAbstractItemView::NoSelection );
		setSelectionBehavior( QAbstractItemView::SelectRows );

		// Delegate.
		const auto delegate = new Delegate::SystemDelegate( p_system, this );
		setItemDelegate( delegate );

		// One expanded at a time.
		// TODO: keep or remove?
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

		// Double click: orient.
		connect(
			delegate,
			&Delegate::SystemDelegate::doubleClicked,
			[ this ]( const QModelIndex & p_index ) { App::ACTION().execute<App::Action::Camera::Orient>(); }
		);

		// Click on visibility icon.
		connect(
			delegate,
			&Delegate::SystemDelegate::visibilityClicked,
			[ this ]( const QModelIndex & p_index )
			{
				const auto &				model = getSystemModel();
				Core::Struct::E_SYSTEM_ITEM item;
				Index						index;
				Model::SystemModel::unpack( p_index.internalId(), item, index );

				const App::System::E_VISIBLE_STATE visible = static_cast<App::System::E_VISIBLE_STATE>(
					p_index.data( Model::SystemModel::Roles::VisibleRole ).toInt()
				);

				App::ACTION().execute<App::Action::Visibility::SetVisibleItem>(
					_system, item, index, visible == App::System::E_VISIBLE_STATE::HIDDEN ? true : false
				);
			}
		);

		// Click on color scheme icon.
		connect(
			delegate,
			&Delegate::SystemDelegate::colorSchemeClicked,
			[ this ]( const QModelIndex & p_index )
			{
				const auto &				model = getSystemModel();
				Core::Struct::E_SYSTEM_ITEM item;
				Index						index;
				Model::SystemModel::unpack( p_index.internalId(), item, index );

				// Get current scheme.
				std::optional<App::System::E_COLOR_SCHEME> scheme
					= App::Helper::System::getColorScheme( { _system, item, index } );

				Menu::ColorScheme menu( this, scheme );
				if ( const auto * res = menu.exec( QCursor::pos() ) )
				{
					// Get menu selection.
					const auto selected = res->data().value<Menu::ColorScheme::Selected>();
					App::ACTION().execute<App::Action::Color::AddItem>(
						_system, item, selected.scheme, index, selected.index
					);
				}
			}
		);

		// Click on representation icon.
		connect(
			delegate,
			&Delegate::SystemDelegate::representationClicked,
			[ this ]( const QModelIndex & p_index )
			{
				const auto &				model = getSystemModel();
				Core::Struct::E_SYSTEM_ITEM item;
				Index						index;
				Model::SystemModel::unpack( p_index.internalId(), item, index );

				// Get current representation.
				std::optional<App::ECS::Entity> representation
					= App::Helper::System::getRepresentation( { _system, item, index } );

				Menu::Representation menu( this, representation );
				if ( const auto * res = menu.exec( QCursor::pos() ) )
				{
					// Get menu selection.
					const auto selected = res->data().value<App::ECS::Entity>();
					App::ACTION().execute<App::Action::Representation::AddItem>( _system, item, selected, index );
				}
			}
		);
	}

	void System::contextMenuEvent( QContextMenuEvent * p_e )
	{
		Menu::Selection menu( this );
		menu.exec( p_e->globalPos() );
	}

	void System::mousePressEvent( QMouseEvent * p_e )
	{
		const QModelIndex index = indexAt( p_e->pos() );
		if ( index.isValid() && p_e->button() == Qt::LeftButton )
		{
			QStyleOptionViewItem option;
			if ( _shouldHandleSelectionClick( p_e, index, option ) )
			{
				if ( not SETTINGS().value( SETTING_KEY_LOCK_SELECTION, false ).toBool() )
				{
					const bool shift  = p_e->modifiers() & Qt::ShiftModifier;
					const bool append = p_e->modifiers() & Qt::ControlModifier;

					if ( shift )
					{
						if ( not _anchor.isValid() )
						{
							_anchor = index;
						}

						_selectVisibleRange( _anchor, index );
					}
					else
					{
						const bool selected = not append or not _isFullySelected( index );
						_applySelection( index, selected, append );
						_anchor = index;

						_dragging		 = true;
						_lastDraggedIndex = index;
					}
				}

				setCurrentIndex( index );
			}
		}

		QTreeView::mousePressEvent( p_e );
	}

	void System::mouseMoveEvent( QMouseEvent * p_e )
	{
		if ( _dragging == false || not( p_e->buttons() & Qt::LeftButton )
			 || SETTINGS().value( SETTING_KEY_LOCK_SELECTION, false ).toBool() )
		{
			QTreeView::mouseMoveEvent( p_e );
			return;
		}

		const QModelIndex index = indexAt( p_e->pos() );
		if ( index.isValid() && index != _lastDraggedIndex )
		{
			QStyleOptionViewItem option;
			if ( _shouldHandleSelectionClick( p_e, index, option ) )
			{
				_applySelection( index, true, true );
				_lastDraggedIndex = index;
				setCurrentIndex( index );
			}
		}

		QTreeView::mouseMoveEvent( p_e );
	}

	void System::mouseReleaseEvent( QMouseEvent * p_e )
	{
		_dragging		 = false;
		_lastDraggedIndex = QPersistentModelIndex();
		QTreeView::mouseReleaseEvent( p_e );
	}

	bool System::_shouldHandleSelectionClick(
		QMouseEvent *				p_e,
		const QModelIndex &			p_index,
		QStyleOptionViewItem &		p_option
	) const
	{
		initViewItemOption( &p_option );
		p_option.rect			  = visualRect( p_index );
		const bool isBranchClick = p_e->pos().x() < p_option.rect.left();

		const auto * delegate = static_cast<const Delegate::SystemDelegate *>( itemDelegateForIndex( p_index ) );
		return delegate != nullptr && isBranchClick == false && not delegate->hitsButton( p_option, p_e->pos() );
	}

	bool System::_isFullySelected( const QModelIndex & p_index ) const
	{
		using namespace App;
		using namespace Core::Struct;

		E_SYSTEM_ITEM item;
		Index		  itemIndex;
		Model::SystemModel::unpack( p_index.internalId(), item, itemIndex );

		return App::Helper::System::getSelectionState( { _system, item, itemIndex } )
			   == App::System::E_SELECTION_STATE::FULL;
	}

	void System::_applySelection( const QModelIndex & p_index, const bool p_selected, const bool p_append )
	{
		using namespace App;
		using namespace Core::Struct;

		E_SYSTEM_ITEM item;
		Index		  itemIndex;
		Model::SystemModel::unpack( p_index.internalId(), item, itemIndex );

		switch ( item )
		{
		case E_SYSTEM_ITEM::SYSTEM:
			SELECTION().select<E_SYSTEM_ITEM::SYSTEM>( _system, Core::Struct::IndexRangeList(), p_selected, p_append );
			break;
		case E_SYSTEM_ITEM::CHAIN: SELECTION().select<E_SYSTEM_ITEM::CHAIN>( _system, itemIndex, p_selected, p_append ); break;
		case E_SYSTEM_ITEM::RESIDUE:
			SELECTION().select<E_SYSTEM_ITEM::RESIDUE>( _system, itemIndex, p_selected, p_append );
			break;
		case E_SYSTEM_ITEM::ATOM: SELECTION().select<E_SYSTEM_ITEM::ATOM>( _system, itemIndex, p_selected, p_append ); break;
		default: break;
		}
	}

	void System::_selectVisibleRange( const QModelIndex & p_from, const QModelIndex & p_to )
	{
		if ( not p_from.isValid() || not p_to.isValid() )
		{
			return;
		}

		QModelIndex current = p_from;
		_applySelection( current, true, false );

		while ( current.isValid() && current != p_to )
		{
			current = _stepToward( *this, current, p_to );
			if ( current.isValid() )
			{
				_applySelection( current, true, true );
			}
		}
	}

} // namespace VTX::UI::QT::Widget::Tree
