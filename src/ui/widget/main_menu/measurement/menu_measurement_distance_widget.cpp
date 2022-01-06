#include "menu_measurement_distance_widget.hpp"
#include "action/action_manager.hpp"
#include "action/measurement.hpp"
#include "controller/measurement_picker.hpp"
#include "id.hpp"
#include "model/atom.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::MainMenu::Measurement
{
	MenuMeasurementDistanceWidget::MenuMeasurementDistanceWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::PICKER_MODE_CHANGE );
	}

	MenuMeasurementDistanceWidget::~MenuMeasurementDistanceWidget() {}

	void MenuMeasurementDistanceWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::PICKER_MODE_CHANGE )
			_refreshButtons();
	}

	void MenuMeasurementDistanceWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		setTitle( "Measurement" );

		_distanceModeButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "distanceModeButton" );
		_distanceModeButton->setData( "Distance", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_distanceModeButton, 0 );

		_distanceToCycleModeButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "distanceToCycleModeButton" );
		_distanceToCycleModeButton->setData(
			"Distance to cycle", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_distanceToCycleModeButton, 0 );

		_angleModeButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "angleModeButton" );
		_angleModeButton->setData( "Angle", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_angleModeButton, 1 );

		_dihedralAngleModeButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "dihedralAngleModeButton" );
		_dihedralAngleModeButton->setData(
			"Dihedral angle", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_dihedralAngleModeButton, 1 );

		validate();

		_refreshButtons();
	}
	void MenuMeasurementDistanceWidget::_setupSlots()
	{
		_distanceModeButton->setTriggerAction( this, &MenuMeasurementDistanceWidget::_applyDistanceModeAction );
		_distanceToCycleModeButton->setTriggerAction( this,
													  &MenuMeasurementDistanceWidget::_applyDistanceToCycleModeAction );
		_angleModeButton->setTriggerAction( this, &MenuMeasurementDistanceWidget::_applyAngleModeAction );
		_dihedralAngleModeButton->setTriggerAction( this,
													&MenuMeasurementDistanceWidget::_applyDihedralAngleModeAction );
	}
	void MenuMeasurementDistanceWidget::localize() {}

	void MenuMeasurementDistanceWidget::_refreshButtons() const
	{
		const State::Visualization * const state
			= VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		if ( state->getCurrentPickerID() == ID::Controller::MEASUREMENT )
		{
			const Controller::MeasurementPicker * const measurementController
				= state->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT );

			const Controller::MeasurementPicker::Mode & mode = measurementController->getCurrentMode();

			_distanceModeButton->showActiveFeedback( mode == Controller::MeasurementPicker::Mode::DISTANCE );
			_distanceToCycleModeButton->showActiveFeedback( mode
															== Controller::MeasurementPicker::Mode::DISTANCE_TO_CYCLE );
			_angleModeButton->showActiveFeedback( mode == Controller::MeasurementPicker::Mode::ANGLE );
			_dihedralAngleModeButton->showActiveFeedback( mode == Controller::MeasurementPicker::Mode::DIHEDRAL_ANGLE );
		}
		else
		{
			_distanceModeButton->showActiveFeedback( false );
			_distanceModeButton->showActiveFeedback( false );
			_distanceModeButton->showActiveFeedback( false );
			_distanceModeButton->showActiveFeedback( false );
		}
	}

	void MenuMeasurementDistanceWidget::_applyDistanceModeAction() const
	{
		_togglePickerState( Controller::MeasurementPicker::Mode::DISTANCE );
	}
	void MenuMeasurementDistanceWidget::_applyDistanceToCycleModeAction() const
	{
		_togglePickerState( Controller::MeasurementPicker::Mode::DISTANCE_TO_CYCLE );
	}
	void MenuMeasurementDistanceWidget::_applyAngleModeAction() const
	{
		_togglePickerState( Controller::MeasurementPicker::Mode::ANGLE );
	}
	void MenuMeasurementDistanceWidget::_applyDihedralAngleModeAction() const
	{
		_togglePickerState( Controller::MeasurementPicker::Mode::DIHEDRAL_ANGLE );
	}

	void MenuMeasurementDistanceWidget::_togglePickerState(
		const Controller::MeasurementPicker::Mode & p_measurementMode ) const
	{
		State::Visualization * const state
			= VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		if ( state->getCurrentPickerID() != ID::Controller::MEASUREMENT )
		{
			state->setPickerController( ID::Controller::MEASUREMENT );

			Controller::MeasurementPicker * const measurementController
				= state->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT );

			measurementController->setCurrentMode( p_measurementMode );
		}
		else
		{
			Controller::MeasurementPicker * const measurementController
				= state->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT );

			if ( measurementController->getCurrentMode() == p_measurementMode )
			{
				state->setPickerController( ID::Controller::PICKER );
			}
			else
			{
				measurementController->setCurrentMode( p_measurementMode );
			}
		}
	}

} // namespace VTX::UI::Widget::MainMenu::Measurement
