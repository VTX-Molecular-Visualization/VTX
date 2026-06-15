#ifndef __VTX_UI_QT_DELEGATE_SYSTEM_DELEGATE__
#define __VTX_UI_QT_DELEGATE_SYSTEM_DELEGATE__

#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <app/ecs.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT::Delegate
{
	/**
	 * @brief Custom delegate for system tree items.
	 */
	class SystemDelegate : public QStyledItemDelegate
	{
		Q_OBJECT

	  public:
		enum struct ACTION : int
		{
			VISIBILITY = 0,
			COLOR_SCHEME,
			REPRESENTATION,
			DISPLAY_MODE,
			COUNT
		};

		/**
		 * @brief Constructor.
		 */
		SystemDelegate( const Entity, QObject * );

		/**
		 * @brief Override: paint the system item with custom visuals.
		 */
		void paint( QPainter *, const QStyleOptionViewItem &, const QModelIndex & ) const override;

		/**
		 * @brief Override: provide custom size hint for system items.
		 */
		QSize sizeHint( const QStyleOptionViewItem &, const QModelIndex & ) const override;

		/**
		 * @brief Override: handle mouse events for the buttons in the system item.
		 */
		bool editorEvent( QEvent *, QAbstractItemModel *, const QStyleOptionViewItem &, const QModelIndex & ) override;

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

		/**
		 * @brief Check if the given point overlaps an action button.
		 */
		bool hitsButton( const QStyleOptionViewItem &, const QModelIndex &, const QPoint & ) const;

	  signals:
		/**
		 * @brief Button clicked.
		 */
		void visibilityClicked( const QModelIndex & );
		void colorSchemeClicked( const QModelIndex & );
		void representationClicked( const QModelIndex & );
		void displayModeClicked( const QModelIndex & );
		void doubleClicked( const QModelIndex & );

	  private:
		/**
		 * @brief Entity to display.
		 */
		Entity _system;

		static constexpr int ICON_SIZE = 15;
		static constexpr int SPACING   = 4;
		static constexpr int MARGIN_R  = 6;

		QRect _buttonsRect( const QStyleOptionViewItem &, const QModelIndex & ) const;
		QRect _buttonRect( const QStyleOptionViewItem &, const QModelIndex &, const int ) const;
		int	  _hitTestButton( const QStyleOptionViewItem &, const QModelIndex &, const QPoint & ) const;
		int	  _buttonCount( const QModelIndex & ) const;
		bool  _isSystemItem( const QModelIndex & ) const;
		QIcon _displayModeIcon( const QModelIndex & ) const;

		std::array<QIcon, toUnderlying( ACTION::COUNT )> _icons;
	};

} // namespace VTX::UI::QT::Delegate
#endif
