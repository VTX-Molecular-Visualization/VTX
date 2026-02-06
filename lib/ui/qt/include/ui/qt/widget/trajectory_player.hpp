#ifndef __VTX_UI_QT_WIDGET_TRAJECTORY_PLAYER__
#define __VTX_UI_QT_WIDGET_TRAJECTORY_PLAYER__

#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QWidget>
#include <app/ecs.hpp>

namespace VTX::UI::QT::Widget
{

	/**
	 * @brief Widget for controlling trajectory playback.
	 * Displays play/pause, stop buttons, a slider for seeking, and frame counter.
	 */
	class TrajectoryPlayer : public QWidget
	{
		Q_OBJECT

	  public:
		explicit TrajectoryPlayer( App::ECS::Entity p_system, QWidget * p_parent = nullptr );

	  private:
		App::ECS::Entity _system;

		QPushButton * _btnPlayPause;
		QPushButton * _btnStop;
		QSlider *	  _slider;
		QLabel *	  _frameLabel;

		bool _isRefreshing = false; // Prevent feedback loops between _refresh() and valueChanged

		void _onPlayPauseClicked();
		void _onStopClicked();
		void _onSliderValueChanged( int p_value );
		void _onTrajectoryUpdated( App::ECS::Registry &, App::ECS::Entity p_entity );

		void _refresh();
		void _updatePlayPauseIcon();
	};

} // namespace VTX::UI::QT::Widget

#endif
