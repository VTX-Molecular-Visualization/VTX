#include "ui/qt/widget/tree/system.hpp"
#include "ui/qt/delegate/system_delegate.hpp"
#include "ui/qt/menu/color_scheme.hpp"
#include "ui/qt/menu/selection.hpp"
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>
#include <app/action/visibility.hpp>
#include <app/services.hpp>
#include <app/system/trajectory.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	System::System( const App::ECS::Entity p_system, QWidget * p_parent ) : QWidget( p_parent ), _system( p_system )
	{
		_layout = new QVBoxLayout( this );
		_layout->setContentsMargins( 0, 0, 0, 0 );
		_layout->setSpacing( 0 );

		// Create trajectory player if system already has multi-frame trajectory
		if ( App::System::hasMultiFrameTrajectory( p_system ) )
		{
			_trajectoryPlayer = new TrajectoryPlayer( p_system, this );
			_layout->addWidget( _trajectoryPlayer );
		}

		// Listen for trajectory creation to add player dynamically
		App::REG().on_construct<App::System::TrajectoryFullBuffer>().connect<&System::_onTrajectoryCreated>( this );

		// Create inner tree
		_tree = new InnerTree( this );
		_tree->setExpandsOnDoubleClick( false );
		_tree->setMouseTracking( true );

		// Model.
		_tree->setModel( new SystemModel( p_system, this ) );

		// Selection.
		_tree->setSelectionModel( new SystemSelectionModel( p_system, _tree->model(), this ) );
		_tree->setSelectionBehavior( QAbstractItemView::SelectRows );

		// Delegate.
		auto * const delegate = new Delegate::SystemDelegate( p_system, this );
		_tree->setItemDelegate( delegate );

		_layout->addWidget( _tree );

		// One expanded at a time.
		// TODO: keep or remove?
		connect(
			_tree,
			&QTreeView::expanded,
			[ this ]( const QModelIndex & p_index )
			{
				QModelIndex parent	 = p_index.parent();
				const int	rowCount = _tree->model()->rowCount( parent );

				for ( int r = 0; r < rowCount; r++ )
				{
					QModelIndex sibling = _tree->model()->index( r, 0, parent );

					if ( sibling != p_index && _tree->isExpanded( sibling ) )
					{
						_tree->collapse( sibling );
					}
				}
			}
		);

		// Double click: orient.
		connect(
			_tree,
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

				const App::System::E_COLOR_SCHEME colorScheme = static_cast<App::System::E_COLOR_SCHEME>(
					p_index.data( Widget::Tree::SystemModel::Roles::ColorSchemeRootRole ).toInt()
				);

				Menu::ColorScheme menu( this );
				auto *			  res = menu.exec( QCursor::pos() );
				if ( res != nullptr ) {}
			}
		);
	}

	void System::InnerTree::contextMenuEvent( QContextMenuEvent * p_e )
	{
		Menu::Selection menu( this );
		menu.exec( p_e->globalPos() );
	}

	void System::_onTrajectoryCreated( App::ECS::Registry &, App::ECS::Entity p_entity )
	{
		// Only handle if this is our system and we don't already have a player
		if ( p_entity != _system || _trajectoryPlayer != nullptr )
		{
			return;
		}

		// Create the trajectory player and insert it at the top
		_trajectoryPlayer = new TrajectoryPlayer( _system, this );
		_layout->insertWidget( 0, _trajectoryPlayer );
	}

} // namespace VTX::UI::QT::Widget::Tree
