#include "ui/qt/widget/tree/system.hpp"
#include "ui/qt/delegate/system_delegate.hpp"
#include "ui/qt/menu/color_scheme.hpp"
#include "ui/qt/menu/representation.hpp"
#include "ui/qt/menu/selection.hpp"
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>
#include <app/action/color.hpp>
#include <app/action/representation.hpp>
#include <app/action/visibility.hpp>
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <app/system/trajectory.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	System::System( const App::ECS::Entity p_system, QWidget * p_parent ) :
		Widget::Tree::BaseTree<System, QTreeView>( p_parent ), _system( p_system )
	{
		setExpandsOnDoubleClick( false );
		setMouseTracking( true );
		setModel( new SystemModel( p_system, this ) );

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
			this,
			&QTreeView::doubleClicked,
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
				SystemModel::unpack( p_index.internalId(), item, index );

				const App::System::E_VISIBLE_STATE visible = static_cast<App::System::E_VISIBLE_STATE>(
					p_index.data( Widget::Tree::SystemModel::Roles::VisibleRole ).toInt()
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
				SystemModel::unpack( p_index.internalId(), item, index );

				// Get current scheme.
				std::optional<App::System::E_COLOR_SCHEME> scheme
					= App::Helper::System::getColorScheme( { _system, item, index } );

				Menu::ColorScheme menu( this, scheme );
				if ( const auto * res = menu.exec( QCursor::pos() ) )
				{
					// Get menu selection.
					const auto selected = res->data().value<Menu::ColorScheme::Selected>();
					// TODO: handle index for plain color.
					App::ACTION().execute<App::Action::Color::AddItem>( _system, item, selected.scheme, index );
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
				SystemModel::unpack( p_index.internalId(), item, index );

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

} // namespace VTX::UI::QT::Widget::Tree
