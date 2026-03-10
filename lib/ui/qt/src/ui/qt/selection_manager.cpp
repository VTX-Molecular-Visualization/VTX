#include "ui/qt/selection_manager.hpp"
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

				if ( p_group != E_SELECTION_GROUP::SYSTEM )
				{
					App::ACTION().execute<App::Action::Selection::Clear>();
				}

				emit selected( p_group, p_view->selection() );
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

	void SelectionManager::clear( const E_SELECTION_GROUP p_group )
	{
		if ( _views.contains( p_group ) )
		{
			for ( QItemSelectionModel * v : _views[ p_group ] )
			{
				if ( v->hasSelection() )
					_clear( v );
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
} // namespace VTX::UI::QT
