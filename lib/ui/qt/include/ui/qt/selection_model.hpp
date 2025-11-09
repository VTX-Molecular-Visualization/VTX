#ifndef __VTX_UI_QT_SELECTION_MODEL__
#define __VTX_UI_QT_SELECTION_MODEL__

#include <QAbstractItemModel>
#include <QItemSelectionModel>

namespace VTX::UI::QT
{
	/**
	 * @brief Selection model representing the application selection.
	 */
	class SelectionModel : QItemSelectionModel
	{
	  public:
		SelectionModel( QAbstractItemModel * p_model ) : QItemSelectionModel( p_model ) {}
	};
} // namespace VTX::UI::QT

#endif
