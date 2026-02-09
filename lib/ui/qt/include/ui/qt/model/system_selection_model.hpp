#ifndef __VTX_UI_QT_SELECTION_MODEL__
#define __VTX_UI_QT_SELECTION_MODEL__

#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <app/ecs.hpp>

namespace VTX::UI::QT::Model
{
	/**
	 * @brief Selection model representing the application selection.
	 */
	class SystemSelectionModel : public QItemSelectionModel
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		SystemSelectionModel( const App::ECS::Entity, QAbstractItemModel *, QObject * = nullptr );

		/**
		 * @brief Refresh from app.
		 */
		void refresh();

	  private:
		/**
		 * @brief Entity of fetch data from.
		 */
		const App::ECS::Entity _system;

		/**
		 * @brief QSelectionModel handler.
		 */
		void _selectionChanged( const QItemSelection &, const QItemSelection & );
	};
} // namespace VTX::UI::QT::Model

#endif
