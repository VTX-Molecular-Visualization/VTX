#ifndef __VTX_UI_QT_DELEGATE_SCENE_ITEM_DELEGATE__
#define __VTX_UI_QT_DELEGATE_SCENE_ITEM_DELEGATE__

#include <QStyledItemDelegate>
#include <app/ecs.hpp>

namespace VTX::UI::QT::Delegate
{

	/**
	 * @brief Custom delegate for scene tree items that renders an inline trajectory player
	 * below system items that have a multi-frame trajectory.
	 */
	class SceneItemDelegate : public QStyledItemDelegate
	{
		Q_OBJECT

	  public:
		explicit SceneItemDelegate( const App::ECS::Entity, QObject * p_parent = nullptr );

		void paint(
			QPainter *					 p_painter,
			const QStyleOptionViewItem & p_option,
			const QModelIndex &			 p_index
		) const override;
		QSize sizeHint( const QStyleOptionViewItem & p_option, const QModelIndex & p_index ) const override;
		bool  editorEvent(
			 QEvent *					  p_event,
			 QAbstractItemModel *		  p_model,
			 const QStyleOptionViewItem & p_option,
			 const QModelIndex &		  p_index
		 ) override;

	  private:
		/**
		 * @brief Entity to display.
		 */
		const App::ECS::Entity _system;

		/**
		 * @brief Check if the given index represents a system with a multi-frame trajectory.
		 */
		bool _hasTrajectory( const QModelIndex & p_index ) const;

		/**
		 * @brief Paint the player controls (play/pause, stop, slider, frame counter).
		 */
		void _paintPlayerControls(
			QPainter *			p_painter,
			const QRect &		p_rect,
			const QModelIndex & p_index,
			bool				p_isSelected
		) const;

		/**
		 * @brief Hit zones for player controls.
		 */
		enum class HitZone
		{
			None,
			PlayPause,
			Stop,
			Slider,
			FrameSelector
		};

		/**
		 * @brief Determine which player control was hit by a mouse click.
		 */
		HitZone _hitTest( const QPoint & p_pos, const QRect & p_controlsRect ) const;

		/**
		 * @brief Get the rectangle for the player controls area.
		 */
		QRect _getControlsRect( const QStyleOptionViewItem & p_option ) const;

		void _updateSliderBar( App::ECS::Entity );

		/**
		 * @brief Layout constants.
		 */
		static constexpr int TEXT_ROW_HEIGHT = 20; // Height for the system name row
		static constexpr int PLAYER_HEIGHT	 = 24;
		static constexpr int BUTTON_SIZE	 = 20;
		static constexpr int SPACING		 = 4;
		static constexpr int FRAME_WIDTH	 = 40;
	};

} // namespace VTX::UI::QT::Delegate

#endif
