#ifndef __VTX_UI_QT_DELEGATE_SYSTEM_DELEGATE__
#define __VTX_UI_QT_DELEGATE_SYSTEM_DELEGATE__

#include <QStyledItemDelegate>
#include <app/ecs.hpp>

namespace VTX::UI::QT::Delegate
{
	/**
	 * @brief Custom delegate for system tree items.
	 */
	class SystemDelegate : public QStyledItemDelegate
	{
	  public:
		/**
		 * @brief Constructor.
		 */
		SystemDelegate( const App::ECS::Entity, QObject * );

		/**
		 * @brief Override: paint the system item with custom visuals.
		 */
		void paint( QPainter *, const QStyleOptionViewItem &, const QModelIndex & ) const override;

		/**
		 * @brief Override: provide custom size hint for system items.
		 */
		QSize sizeHint( const QStyleOptionViewItem &, const QModelIndex & ) const override;

		/**
		 * @brief Override: create custom editor widget for system items (if needed).
		 * For now, we will not provide any editor, but this can be extended in the future.
		 */
		QWidget * createEditor( QWidget *, const QStyleOptionViewItem &, const QModelIndex & ) const override;

		/**
		 * @brief Override: set data from the model into the editor widget (if any).
		 */
		void setEditorData( QWidget *, const QModelIndex & ) const override;

		/**
		 * @brief Override: set data from the editor widget back into the model (if any).
		 */
		void setModelData( QWidget *, QAbstractItemModel *, const QModelIndex & ) const override;

	  private:
		/**
		 * @brief Entity to display.
		 */
		App::ECS::Entity _system;
	};

} // namespace VTX::UI::QT::Delegate
#endif
