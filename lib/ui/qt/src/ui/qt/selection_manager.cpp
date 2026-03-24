#include "ui/qt/selection_manager.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include <QGuiApplication>
#include <app/action/action_manager.hpp>
#include <app/action/selection.hpp>
#include <app/services.hpp>

namespace
{
	void _clear( QItemSelectionModel * const p_view )
	{
		QSignalBlocker blocker( p_view );
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

		connect(
			p_view,
			&QItemSelectionModel::selectionChanged,
			this,
			[ this, p_group, p_view ]( const QItemSelection &, const QItemSelection & )
			{
				clearBut( p_group );
				App::ACTION().execute<App::Action::Selection::Clear>();

				emit selectionChanged( p_group, p_view->selection() );
			}
		);
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

		if ( not SETTINGS().value( SETTING_KEY_LOCK_SELECTION, false ).toBool() )
		{
			App::ACTION().execute<App::Action::Selection::Pick>(
				p_pos,
				static_cast<App::Action::Selection::E_GRANULARITY>(
					SETTINGS().value( SETTING_KEY_GRANULARITY, 0 ).toInt()
				),
				p_append
			);
		}
	}
} // namespace VTX::UI::QT
