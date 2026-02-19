#ifndef __VTX_UI_QT_SELECTION_MANAGER__
#define __VTX_UI_QT_SELECTION_MANAGER__

#include <QItemSelectionModel>
#include <QObject>
#include <app/action/action_manager.hpp>
#include <app/action/selection.hpp>
#include <app/services.hpp>
#include <core/struct/system.hpp>

namespace VTX::UI::QT
{
	/**
	 * @brief Group of item that can be selected together.
	 */
	enum struct E_SELECTION_GROUP
	{
		GRAPHICS_CONFIG,
		COLOR_LAYOUT,
		REPRESENTATION,
		CAMERA,
		SYSTEM,
	};

	/**
	 * @brief QSelectionModel manager.
	 */
	class SelectionManager : public QObject
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Constructor.
		 */
		SelectionManager( QObject * p_parent ) : QObject( p_parent ) {}

		/**
		 * @brief Add a view to the manager.
		 */
		void add( QItemSelectionModel * p_view, const E_SELECTION_GROUP p_group )
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
				[ this, p_group ]( const QItemSelection &, const QItemSelection & )
				{
					// const Qt::KeyboardModifiers mods	= QGuiApplication::keyboardModifiers();
					// const bool					isCtrl	= mods & Qt::ControlModifier;
					// const bool					isShift = mods & Qt::ShiftModifier;

					// Unselect others.
					for ( auto & [ group, views ] : _views )
					{
						if ( group != p_group /* || ( not isCtrl && not isShift ) */ )
						{
							for ( QItemSelectionModel * v : views )
							{
								QSignalBlocker blocker( v );
								v->clearSelection();
							}
						}
					}
				}
			);
		}

		template<Core::Struct::E_SYSTEM_ITEM ITEM>
		void selectSystem(
			const App::ECS::Entity p_system,
			const Index			   p_index,
			const bool			   p_select = true,
			const bool			   p_append = false
		)
		{
			using namespace App;
			using namespace Core::Struct;

			ACTION().execute<App::Action::Selection::SetSelected<ITEM>>( p_system, p_index, p_select );

			// Emit.
		}

		/**
		 * @brief Clear selection of a group.
		 */
		void _clear( const E_SELECTION_GROUP p_group )
		{
			if ( _views.contains( p_group ) )
			{
				for ( QItemSelectionModel * v : _views[ p_group ] )
				{
					QSignalBlocker blocker( v );
					v->clearSelection();
				}
			}
		}

		/**
		 * @brief Clear all selection.
		 */
		void _clear()
		{
			for ( auto & [ group, views ] : _views )
			{
				for ( QItemSelectionModel * v : views )
				{
					QSignalBlocker blocker( v );
					v->clearSelection();
				}
			}
		}

		void _selectionChanged( const QItemSelection & p_selected, const QItemSelection & p_deselected );

	  signals:
		void presetSelectionChanged( const QItemSelection & p_selection );
		void systemSelectionChanged( const QItemSelection & p_selection );

	  private:
		/**
		 * @brief Grouped selection models.
		 */
		std::unordered_map<E_SELECTION_GROUP, std::vector<QItemSelectionModel *>> _views;
	};
} // namespace VTX::UI::QT

#endif
