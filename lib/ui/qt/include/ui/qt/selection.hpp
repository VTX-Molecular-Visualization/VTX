#ifndef __VTX_UI_QT_SELECTION__
#define __VTX_UI_QT_SELECTION__

#include <QAbstractItemModel>
#include <QItemSelectionModel>

// TODO: connect with app selection model.

namespace VTX::UI::QT
{
	/**
	 * @brief Selection model representing selection in App.
	 */
	class Selection : QItemSelectionModel
	{
	  public:
		Selection( QAbstractItemModel * p_model ) : QItemSelectionModel( p_model ) {}
	};
} // namespace VTX::UI::QT

#endif
