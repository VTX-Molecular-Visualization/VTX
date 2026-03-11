#ifndef __VTX_UI_QT_SELECTION_MANAGER__
#define __VTX_UI_QT_SELECTION_MANAGER__

#include <QAbstractItemView>
#include <QItemSelectionModel>
#include <QObject>

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
		void add( QItemSelectionModel *, const E_SELECTION_GROUP );

		/**
		 * @brief Clear all selection.
		 */
		void clear();

		/**
		 * @brief Clear selection of a group.
		 */
		void clear( const E_SELECTION_GROUP );

		/**
		 * @brief Clear selection of all group except the given one.
		 */
		void clearBut( const E_SELECTION_GROUP );

	  signals:
		/**
		 * @brief Selected signal.
		 */
		void selected( const E_SELECTION_GROUP, const QItemSelection & p_selection );

		/**
		 * @brief Clear selection.
		 */
		void cleared( const E_SELECTION_GROUP );

	  private:
		/**
		 * @brief Grouped selection models.
		 */
		std::unordered_map<E_SELECTION_GROUP, std::vector<QItemSelectionModel *>> _views;
	};
} // namespace VTX::UI::QT

#endif
