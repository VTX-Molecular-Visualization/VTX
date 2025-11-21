#ifndef __VTX_UI_QT_SELECTION_MODEL__
#define __VTX_UI_QT_SELECTION_MODEL__

#include <QAbstractItemModel>
#include <QItemSelectionModel>

namespace VTX::UI::QT
{
	/**
	 * @brief Selection model representing the application selection.
	 */
	class SelectionModel : public QItemSelectionModel
	{
		Q_OBJECT

	  public:
		SelectionModel( QAbstractItemModel * p_model, QObject * p_parent = nullptr );

	  private:
		void _selectionChanged( const QItemSelection &, const QItemSelection & );
	};
} // namespace VTX::UI::QT

#endif
