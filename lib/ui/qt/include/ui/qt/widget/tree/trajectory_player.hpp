#ifndef __VTX_UI_QT_WIDGET_TRAJECTORY_PLAYER__
#define __VTX_UI_QT_WIDGET_TRAJECTORY_PLAYER__

#include "ui/qt/widget/tree/trajectory_settings.hpp"
#include "ui/qt/widget/tree/trajectory_slider.hpp"
#include <QIcon>
#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QWidget>
#include <app/ecs.hpp>
#include <array>

namespace VTX::UI::QT::Widget::Tree
{

	/**
	 * @brief Widget for controlling trajectory playback.
	 * Displays play/pause, stop buttons, a slider for seeking, and frame counter.
	 * Contains a collapsible settings panel.
	 */
	class TrajectoryPlayer : public QWidget
	{
		Q_OBJECT

	  public:
		explicit TrajectoryPlayer( App::Entity p_system, QWidget * p_parent = nullptr );

	  private:
		static std::array<QIcon, 4> _getIcons();

		App::Entity _system;

		QPointer<QPushButton>		_btnPlayPause;
		QPointer<QPushButton>		_btnStop;
		QPointer<QPushButton>		_btnSettings;
		QPointer<TrajectorySlider>	_slider;
		QPointer<QLabel>			_frameLabel;
		QPointer<TrajectorySettings> _settings;
		std::array<QIcon, 4>		_icons { _getIcons() };

		bool _isRefreshing = false; // Prevent feedback loops between _refresh() and valueChanged

		void _onPlayPauseClicked();
		void _onStopClicked();
		void _onSettingsClicked();
		void _onSliderValueChanged( int p_value );
		void _onTrajectoryUpdated( App::Registry &, App::Entity p_entity );

		void _refresh();
		void _updatePlayPauseIcon();
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
