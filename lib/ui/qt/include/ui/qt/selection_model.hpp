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
		SelectionModel( QAbstractItemModel * p_model, QObject * p_parent = nullptr ) :
			QItemSelectionModel( p_model, p_parent )
		{
			// TODO: connect to application selection changes.
			// TODO: propagate selection changes to application selection.
		}
	};
} // namespace VTX::UI::QT

#endif
