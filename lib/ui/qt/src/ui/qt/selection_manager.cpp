#include "ui/qt/selection_manager.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include <QGuiApplication>
#include <app/action/selection.hpp>
#include <app/services.hpp>
#include <util/action_manager.hpp>

namespace
{
	void _clear( QItemSelectionModel * const p_view )
	{
		const QSignalBlocker blocker( p_view );
		p_view->clearSelection();
		if ( auto * view = qobject_cast<QAbstractItemView *>( p_view->parent() ) )
		{
			view->viewport()->update();
		}
	}
} // namespace

namespace VTX::UI::QT
{
	void SelectionManager::add( QItemSelectionModel * p_view, const E_SELECTION_GROUP p_group )
	{
		if ( not _views.contains( p_group ) )
		{
			_views[ p_group ] = std::vector<QItemSelectionModel *>();
		}
		_views[ p_group ].push_back( p_view );

		connect( p_view,
				 &QItemSelectionModel::selectionChanged,
				 this,
				 [ this, p_group, p_view ]( const QItemSelection &, const QItemSelection & )
				 {
					 clearBut( p_group );
					 App::ACTION().execute<App::Action::Selection::Clear>();

					 emit selectionChanged( p_group, p_view->selection() );
				 } );
	}

	void SelectionManager::clear()
	{
		for ( auto & [ group, _ ] : _views )
		{
			clear( group );
		}
	}

	void SelectionManager::clearSystem()
	{
		clear();
		App::ACTION().execute<App::Action::Selection::Clear>();
	}

	void SelectionManager::clear( const E_SELECTION_GROUP p_group )
	{
		if ( _views.contains( p_group ) )
		{
			bool changed = false;
			for ( QItemSelectionModel * v : _views[ p_group ] )
			{
				if ( v->hasSelection() )
				{
					_clear( v );
					changed = true;
				}
			}

			if ( changed )
			{
				emit selectionChanged( p_group, QItemSelection() );
			}
		}
	}

	void SelectionManager::selectPreset( const E_SELECTION_GROUP p_group, const Entity p_preset )
	{
		const auto it = _views.find( p_group );
		if ( it == _views.end() )
		{
			return;
		}

		for ( QItemSelectionModel * const view : it->second )
		{
			const QAbstractItemModel * const model	 = view->model();
			const QModelIndexList			 matches = model->match(
				   model->index( 0, 0 ),
				   Qt::UserRole,
				   QVariant::fromValue<Entity>( p_preset ),
				   1,
				   Qt::MatchExactly | Qt::MatchRecursive
			   );
			if ( not matches.isEmpty() )
			{
				view->setCurrentIndex( matches.first(), QItemSelectionModel::ClearAndSelect );
			}
		}
	}

	void SelectionManager::clearBut( const E_SELECTION_GROUP p_group )
	{
		for ( auto & [ group, _ ] : _views )
		{
			if ( group != p_group )
			{
				clear( group );
			}
		}
	}

	void SelectionManager::pick( const Vec2i & p_pos, const bool p_append = false )
	{
		clear();

		if ( not UI_ACTIONS().isChecked( Action::Selection::LOCK ) )
		{
			App::ACTION().execute<App::Action::Selection::Pick>(
				p_pos,
				static_cast<App::Action::Selection::E_GRANULARITY>(
					SETTINGS().value( SETTING_KEY_GRANULARITY, 0 ).toInt() ),
				p_append );
		}
	}
} // namespace VTX::UI::QT
