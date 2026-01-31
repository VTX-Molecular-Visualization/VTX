#include "ui/qt/delegate/scene_item_delegate.hpp"
#include "app/system/trajectory.hpp"
#include "ui/qt/model.hpp"
#include "ui/qt/services.hpp"
#include <QAbstractItemView>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <app/action/action_manager.hpp>
#include <app/action/trajectory.hpp>
#include <app/events.hpp>
#include <app/services.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Delegate
{

	SceneItemDelegate::SceneItemDelegate( QObject * p_parent ) : QStyledItemDelegate( p_parent )
	{
		App::REG().on_update<App::System::TrajectoryFullBuffer>().connect<&SceneItemDelegate::_updateSliderBar>( this );
	}
	void SceneItemDelegate::_updateSliderBar( App::ECS::Entity p_entity )
	{
		// Check if this entity has a trajectory
		App::System::GenericTrajectory * trajPtr = nullptr;
		App::System::get( p_entity, trajPtr );
		if ( trajPtr == nullptr )
			return;

		// Trigger a repaint of the tree view to update the slider position
		// The delegate's parent is the tree view (set in scene.cpp)
		if ( auto * view = qobject_cast<QAbstractItemView *>( parent() ) )
		{
			view->viewport()->update();
		}
	}

	void SceneItemDelegate::paint(
		QPainter *					 p_painter,
		const QStyleOptionViewItem & p_option,
		const QModelIndex &			 p_index
	) const
	{
		// Let the base class handle painting for non-trajectory items
		if ( !_hasTrajectory( p_index ) )
		{
			QStyledItemDelegate::paint( p_painter, p_option, p_index );
			return;
		}

		p_painter->save();

		// Draw selection/hover background for the entire item area
		QStyleOptionViewItem opt = p_option;
		initStyleOption( &opt, p_index );

		// Draw background
		if ( opt.state & QStyle::State_Selected )
		{
			p_painter->fillRect( opt.rect, opt.palette.highlight() );
		}
		else if ( opt.state & QStyle::State_MouseOver )
		{
			p_painter->fillRect( opt.rect, opt.palette.midlight() );
		}

		// Text rect is the top portion with fixed height
		QRect textRect( opt.rect.left() + SPACING, opt.rect.top(), opt.rect.width() - SPACING, TEXT_ROW_HEIGHT );

		// Draw the system name text
		const QString text		 = p_index.data( Qt::DisplayRole ).toString();
		const bool	  isSelected = opt.state & QStyle::State_Selected;
		p_painter->setPen( isSelected ? opt.palette.highlightedText().color() : opt.palette.text().color() );
		p_painter->drawText( textRect, Qt::AlignVCenter | Qt::AlignLeft, text );

		// Player controls rect is below the text
		QRect controlsRect(
			opt.rect.left() + SPACING, opt.rect.top() + TEXT_ROW_HEIGHT, opt.rect.width() - 2 * SPACING, PLAYER_HEIGHT
		);
		_paintPlayerControls( p_painter, controlsRect, p_index, isSelected );

		p_painter->restore();
	}

	QSize SceneItemDelegate::sizeHint( const QStyleOptionViewItem & p_option, const QModelIndex & p_index ) const
	{
		QSize baseSize = QStyledItemDelegate::sizeHint( p_option, p_index );

		if ( _hasTrajectory( p_index ) )
		{
			// Use fixed height: text row + player controls
			baseSize.setHeight( TEXT_ROW_HEIGHT + PLAYER_HEIGHT );
		}

		return baseSize;
	}

	bool SceneItemDelegate::editorEvent(
		QEvent *					 p_event,
		QAbstractItemModel *		 p_model,
		const QStyleOptionViewItem & p_option,
		const QModelIndex &			 p_index
	)
	{
		if ( !_hasTrajectory( p_index ) )
		{
			return QStyledItemDelegate::editorEvent( p_event, p_model, p_option, p_index );
		}

		if ( p_event->type() == QEvent::MouseButtonPress || p_event->type() == QEvent::MouseButtonRelease )
		{
			QMouseEvent * mouseEvent   = static_cast<QMouseEvent *>( p_event );
			QRect		  controlsRect = _getControlsRect( p_option );

			// Only handle clicks in the controls area
			if ( controlsRect.contains( mouseEvent->pos() ) )
			{
				if ( p_event->type() == QEvent::MouseButtonRelease )
				{
					HitZone zone = _hitTest( mouseEvent->pos(), controlsRect );

					// Get the entity for this system
					using namespace App::Scene;
					E_ITEM	item;
					RootUID rootUID;
					Index	localIndex;
					Model::unpack( p_index.internalId(), item, rootUID, localIndex );

					const auto & entityMap = MODEL().getMapRootToEntity();
					if ( !entityMap.contains( rootUID ) )
					{
						return true;
					}

					App::ECS::Entity entity = entityMap.at( rootUID );

					switch ( zone )
					{
					case HitZone::PlayPause:
						App::ACTION().execute<App::Action::Trajectory::ToggleStartPause>( entity );
						break;
					case HitZone::Stop: App::ACTION().execute<App::Action::Trajectory::Stop>( entity ); break;
					case HitZone::Slider:
					{
						// Calculate which frame was clicked
						App::System::GenericTrajectory * trajPtr = nullptr;
						App::System::get( entity, trajPtr );
						if ( trajPtr && trajPtr->trajectorySize > 0 )
						{
							int sliderLeft	= controlsRect.left() + 2 * ( BUTTON_SIZE + SPACING );
							int sliderRight = controlsRect.right() - FRAME_WIDTH - SPACING;
							int sliderWidth = sliderRight - sliderLeft;

							if ( sliderWidth > 0 )
							{
								float ratio = float( mouseEvent->pos().x() - sliderLeft ) / float( sliderWidth );
								ratio		= std::clamp( ratio, 0.0f, 1.0f );
								uint frame	= uint( ratio * float( trajPtr->trajectorySize - 1 ) );
								App::ACTION().execute<App::Action::Trajectory::JumpTo>( entity, frame );
							}
						}
						break;
					}
					case HitZone::FrameSelector:
						// Could open an edit widget for frame input
						break;
					case HitZone::None: break;
					}
				}
				return true; // Event handled
			}
		}

		return QStyledItemDelegate::editorEvent( p_event, p_model, p_option, p_index );
	}

	bool SceneItemDelegate::_hasTrajectory( const QModelIndex & p_index ) const
	{
		using namespace App::Scene;

		if ( !p_index.isValid() )
		{
			return false;
		}

		E_ITEM	item;
		RootUID rootUID;
		Index	localIndex;
		Model::unpack( p_index.internalId(), item, rootUID, localIndex );

		// Only system items can have trajectories
		if ( item != E_ITEM::SYSTEM )
		{
			return false;
		}

		const auto & entityMap = MODEL().getMapRootToEntity();
		if ( !entityMap.contains( rootUID ) )
		{
			return false;
		}

		return App::System::hasMultiFrameTrajectory( entityMap.at( rootUID ) );
	}

	void SceneItemDelegate::_paintPlayerControls(
		QPainter *			p_painter,
		const QRect &		p_rect,
		const QModelIndex & p_index,
		bool				p_isSelected
	) const
	{
		// Get trajectory info for frame display
		using namespace App::Scene;
		E_ITEM	item;
		RootUID rootUID;
		Index	localIndex;
		Model::unpack( p_index.internalId(), item, rootUID, localIndex );

		const auto &					 entityMap = MODEL().getMapRootToEntity();
		App::System::GenericTrajectory * trajPtr   = nullptr;
		if ( entityMap.contains( rootUID ) )
		{
			App::System::get( entityMap.at( rootUID ), trajPtr );
		}

		// Layout: [Play/Pause] [Stop] [====slider====] [frame#]
		int x = p_rect.left();
		int y = p_rect.top();
		int h = p_rect.height();

		QPen textPen = p_isSelected ? p_painter->pen() : QPen( QApplication::palette().text().color() );
		p_painter->setPen( textPen );

		// Play/Pause button
		QRect playRect( x, y + ( h - BUTTON_SIZE ) / 2, BUTTON_SIZE, BUTTON_SIZE );
		p_painter->drawRect( playRect );
		// Draw play triangle or pause bars
		bool isPlaying = trajPtr && not trajPtr->paused;
		if ( isPlaying )
		{
			// Pause bars
			int barW = 3;
			int barH = BUTTON_SIZE - 8;
			int barY = playRect.top() + 4;
			p_painter->fillRect( playRect.left() + 5, barY, barW, barH, textPen.color() );
			p_painter->fillRect( playRect.left() + 12, barY, barW, barH, textPen.color() );
		}
		else
		{
			// Play triangle
			QPolygon triangle;
			triangle << QPoint( playRect.left() + 6, playRect.top() + 4 )
					 << QPoint( playRect.left() + 6, playRect.bottom() - 4 )
					 << QPoint( playRect.right() - 4, playRect.center().y() );
			p_painter->setBrush( textPen.color() );
			p_painter->drawPolygon( triangle );
			p_painter->setBrush( Qt::NoBrush );
		}

		x += BUTTON_SIZE + SPACING;

		// Stop button
		QRect stopRect( x, y + ( h - BUTTON_SIZE ) / 2, BUTTON_SIZE, BUTTON_SIZE );
		p_painter->drawRect( stopRect );
		// Draw stop square
		p_painter->fillRect( stopRect.adjusted( 5, 5, -5, -5 ), textPen.color() );

		x += BUTTON_SIZE + SPACING;

		// Frame display (at the end)
		int	  frameX = p_rect.right() - FRAME_WIDTH;
		QRect frameRect( frameX, y, FRAME_WIDTH, h );
		uint  currentFrame = trajPtr ? trajPtr->currentFrameIndex : 0;
		uint  totalFrames  = trajPtr ? uint( trajPtr->trajectorySize ) : 0;
		if ( currentFrame == std::numeric_limits<uint>::max() )
		{
			currentFrame = 0;
		}
		if ( totalFrames == std::numeric_limits<size_t>::max() )
		{
			totalFrames = 0;
		}
		QString frameText = QString( "%1/%2" ).arg( currentFrame ).arg( totalFrames );
		p_painter->drawText( frameRect, Qt::AlignVCenter | Qt::AlignRight, frameText );

		// Slider (fills remaining space)
		int	  sliderWidth = frameX - x - SPACING;
		QRect sliderRect( x, y + ( h - 6 ) / 2, sliderWidth, 6 );
		p_painter->drawRect( sliderRect );

		// Draw slider position
		if ( totalFrames > 0 )
		{
			float ratio = float( currentFrame ) / float( totalFrames );
			int	  knobX = sliderRect.left() + int( ratio * ( sliderRect.width() - 8 ) );
			QRect knobRect( knobX, sliderRect.top() - 2, 8, 10 );
			p_painter->fillRect( knobRect, textPen.color() );
		}
	}

	SceneItemDelegate::HitZone SceneItemDelegate::_hitTest( const QPoint & p_pos, const QRect & p_controlsRect ) const
	{
		int x = p_controlsRect.left();
		int y = p_controlsRect.top();
		int h = p_controlsRect.height();

		// Play/Pause button
		QRect playRect( x, y + ( h - BUTTON_SIZE ) / 2, BUTTON_SIZE, BUTTON_SIZE );
		if ( playRect.contains( p_pos ) )
		{
			return HitZone::PlayPause;
		}

		x += BUTTON_SIZE + SPACING;

		// Stop button
		QRect stopRect( x, y + ( h - BUTTON_SIZE ) / 2, BUTTON_SIZE, BUTTON_SIZE );
		if ( stopRect.contains( p_pos ) )
		{
			return HitZone::Stop;
		}

		x += BUTTON_SIZE + SPACING;

		// Frame display
		int	  frameX = p_controlsRect.right() - FRAME_WIDTH;
		QRect frameRect( frameX, y, FRAME_WIDTH, h );
		if ( frameRect.contains( p_pos ) )
		{
			return HitZone::FrameSelector;
		}

		// Slider (remaining space)
		int	  sliderWidth = frameX - x - SPACING;
		QRect sliderRect( x, y, sliderWidth, h );
		if ( sliderRect.contains( p_pos ) )
		{
			return HitZone::Slider;
		}

		return HitZone::None;
	}

	QRect SceneItemDelegate::_getControlsRect( const QStyleOptionViewItem & p_option ) const
	{
		// Controls rect is below the text row (using fixed TEXT_ROW_HEIGHT)
		return QRect(
			p_option.rect.left() + SPACING,
			p_option.rect.top() + TEXT_ROW_HEIGHT,
			p_option.rect.width() - 2 * SPACING,
			PLAYER_HEIGHT
		);
	}

} // namespace VTX::UI::QT::Delegate
