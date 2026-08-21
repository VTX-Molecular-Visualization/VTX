#ifndef __VTX_UI_QT_WIDGET_TRAJECTORY_SETTINGS__
#define __VTX_UI_QT_WIDGET_TRAJECTORY_SETTINGS__

#include <QComboBox>
#include <QPointer>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>
#include <app/ecs.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	/**
	 * @brief Collapsible settings panel for trajectory playback configuration.
	 * Contains player mode, speed control, and frame input.
	 */
	class TrajectorySettings : public QWidget
	{
		Q_OBJECT

	  public:
		explicit TrajectorySettings( Entity p_system, QWidget * p_parent = nullptr );
		~TrajectorySettings() override;

	  protected:
		bool eventFilter( QObject * p_watched, QEvent * p_event ) override;

	  private:
		Entity _system;

		QPointer<QComboBox>		 _playerModeCombo;
		QPointer<QSlider>		 _speedSlider;
		QPointer<QDoubleSpinBox> _speedSpinBox;
		QPointer<QSpinBox>		 _frameSpinBox;

		bool _isRefreshing = false;

		void _onPlayerModeChanged( int p_index );
		void _onSpeedSliderChanged( int p_value );
		void _onSpeedSpinBoxChanged( double p_value );
		void _onFrameSpinBoxFocused();
		void _onFrameSpinBoxChanged( int p_value );
		void _onTrajectoryUpdated( Registry &, Entity p_entity );

		void _refresh();
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
