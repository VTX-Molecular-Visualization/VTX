#ifndef __VTX_UI_QT_SELECTION_MANAGER__
#define __VTX_UI_QT_SELECTION_MANAGER__

#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QObject>
#include <app/action/action_manager.hpp>
#include <app/action/selection.hpp>
#include <app/ecs.hpp>
#include <app/services.hpp>
#include <core/struct/topology.hpp>
#include <util/types.hpp>

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
		CAMERA
	};

	/**
	 * @brief QSelectionModel manager.
	 */
	class SelectionManager : public QObject
	{
		Q_OBJECT

	  public:
		/**
		 * @brief Current selection state.
		 */
		/*
		struct Current
		{
			E_SELECTION_GROUP group;
			QModelIndex &	  index;
		};
		*/

		/**
		 * @brief Constructor.
		 */
		SelectionManager( QObject * p_parent ) : QObject( p_parent ) {}

		/**
		 * @brief Add a view to the manager.
		 */
		void add( QItemSelectionModel *, const E_SELECTION_GROUP );

		/**
		 * @brief Clear all selection.
		 */
		void clear();

		/**
		 * @brief Clear the application system selection from the UI.
		 */
		void clearSystem();

		/**
		 * @brief Clear selection of a group.
		 */
		void clear( const E_SELECTION_GROUP );

		/**
		 * @brief Clear selection of all group except the given one.
		 */
		void clearBut( const E_SELECTION_GROUP );

		/**
		 * @brief Select item.
		 */
		template<Core::Struct::E_SYSTEM_ITEM ITEM, typename S>
		void select(
			const App::Entity p_ent,
			const S &			   p_data,
			const bool			   p_selected = true,
			const bool			   p_append	  = false
		)
		{
			using namespace App::Action;

			clear();

			if ( not p_append )
			{
				App::ACTION().execute<App::Action::Selection::Clear>( p_ent, Selection::Clear::E_MODE::OTHERS );
			}

			App::ACTION().execute<App::Action::Selection::SetSelected<ITEM>>( p_ent, p_data, p_selected, p_append );
		}

		/**
		 * @brief Pick item at position and add it to the selection.
		 */
		void pick( const Vec2i &, const bool );

		/**
		 * @brief Get current selection.
		 */
		// inline const std::optional<Current> & getCurrent() const { return _current; }

	  signals:
		/**
		 * @brief UI selection changed signal. Empty selection means no item is selected in the group.

		 */
		void selectionChanged( const E_SELECTION_GROUP, const QItemSelection & p_selection );

	  private:
		/**
		 * @brief Grouped selection models.
		 */
		std::unordered_map<E_SELECTION_GROUP, std::vector<QItemSelectionModel *>> _views;

		/**
		 * @brief Current selection.
		 */
		// std::optional<Current> _current;
	};
} // namespace VTX::UI::QT

#endif
