#ifndef __VTX_UI_QT_SELECTION_MODEL__
#define __VTX_UI_QT_SELECTION_MODEL__

#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <app/events.hpp>

namespace VTX::UI::QT
{
	/**
	 * @brief Selection model representing the application selection.
	 */
	class SelectionModel : public QItemSelectionModel
	{
	  public:
		SelectionModel( QAbstractItemModel * p_model, QObject * p_parent = nullptr );

	  private:
		/**
		 * @brief QSelectionModel handler.
		 */
		void _selectionChanged( const QItemSelection &, const QItemSelection & );

		/**
		 * @brief App
		 */
		void _onSelectionChange( const App::Events::SelectionChange & );
	};
} // namespace VTX::UI::QT

#endif
