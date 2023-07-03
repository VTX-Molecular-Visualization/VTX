#include "tool/old_tool/ui/widget/main_menu/tool/menu_tool_measurement_widget.hpp"
#include "tool/old_tool/action/measurement.hpp"
#include "tool/old_tool/controller/measurement_picker.hpp"
#include <app/old/action/main.hpp>
#include <app/old/event/global.hpp>
#include <app/old/component/chemistry/atom.hpp>
#include <app/old/id.hpp>
#include <ui/old_ui/action/main.hpp>
#include <ui/old_ui/state/state_machine.hpp>
#include <ui/old_ui/state/visualization.hpp>
#include <ui/old_ui/ui/widget_factory.hpp>
#include <ui/old_ui/vtx_app.hpp>

namespace VTX::UI::Widget::MainMenu::Tool
{
	MenuToolMeasurementWidget::MenuToolMeasurementWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( VTX::App::Old::Event::Global::PICKER_MODE_CHANGE );
	}

	MenuToolMeasurementWidget::~MenuToolMeasurementWidget() {}

	void MenuToolMeasurementWidget::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Old::Event::Global::PICKER_MODE_CHANGE )
			_refreshButtons();
	}

	void MenuToolMeasurementWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		setTitle( "Measurement" );

		_distanceModeButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "distanceModeButton" );
		_distanceModeButton->setData( "Distance", ":/sprite/measurement_distance_icon.png", Qt::Orientation::Vertical );
		pushButton( *_distanceModeButton, 0 );

		/*_distanceToCycleModeButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "distanceToCycleModeButton" );
		_distanceToCycleModeButton->setData(
			"Distance\nto cycle", ":/sprite/measurement_distance_to_cycle_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_distanceToCycleModeButton, 0 );*/

		_angleModeButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "angleModeButton" );
		_angleModeButton->setData( "Angle", ":/sprite/measurement_angle_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_angleModeButton, 1 );

		_dihedralAngleModeButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "dihedralAngleModeButton" );
		_dihedralAngleModeButton->setData(
			"Dihedral\nangle", ":/sprite/measurement_dihedral_angle_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_dihedralAngleModeButton, 1 );

		validate();

		_refreshButtons();
	}
	void MenuToolMeasurementWidget::_setupSlots()
	{
		_distanceModeButton->setTriggerAction( this, &MenuToolMeasurementWidget::_applyDistanceModeAction );
		//_distanceToCycleModeButton->setTriggerAction( this,
		//											  &MenuToolMeasurementWidget::_applyDistanceToCycleModeAction );
		_angleModeButton->setTriggerAction( this, &MenuToolMeasurementWidget::_applyAngleModeAction );
		_dihedralAngleModeButton->setTriggerAction( this, &MenuToolMeasurementWidget::_applyDihedralAngleModeAction );
	}
	void MenuToolMeasurementWidget::localize() {}

	void MenuToolMeasurementWidget::_refreshButtons() const
	{
		const State::Visualization * const state
			= UI::VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		if ( state->getCurrentPickerID() == ID::Controller::MEASUREMENT )
		{
			const Controller::MeasurementPicker * const measurementController
				= state->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT );

			const Controller::MeasurementPicker::Mode & mode = measurementController->getCurrentMode();

			_distanceModeButton->showActiveFeedback( mode == Controller::MeasurementPicker::Mode::DISTANCE );
			//_distanceToCycleModeButton->showActiveFeedback( mode
			//												== Controller::MeasurementPicker::Mode::DISTANCE_TO_CYCLE );
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

	void MenuToolMeasurementWidget::_applyDistanceModeAction() const
	{
		_togglePickerState( Controller::MeasurementPicker::Mode::DISTANCE );
	}
	void MenuToolMeasurementWidget::_applyDistanceToCycleModeAction() const
	{
		_togglePickerState( Controller::MeasurementPicker::Mode::DISTANCE_TO_CYCLE );
	}
	void MenuToolMeasurementWidget::_applyAngleModeAction() const
	{
		_togglePickerState( Controller::MeasurementPicker::Mode::ANGLE );
	}
	void MenuToolMeasurementWidget::_applyDihedralAngleModeAction() const
	{
		_togglePickerState( Controller::MeasurementPicker::Mode::DIHEDRAL_ANGLE );
	}

	void MenuToolMeasurementWidget::_togglePickerState(
		const Controller::MeasurementPicker::Mode & p_measurementMode ) const
	{
		State::Visualization * const state
			= UI::VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		if ( state->getCurrentPickerID() != ID::Controller::MEASUREMENT )
		{
			VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::MEASUREMENT, int( p_measurementMode ) ) );
		}
		else
		{
			Controller::MeasurementPicker * const measurementController
				= state->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT );

			if ( measurementController->getCurrentMode() == p_measurementMode )
			{
				VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::PICKER ) );
			}
			else
			{
				VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::MEASUREMENT, int( p_measurementMode ) ) );
			}
		}
	}

} // namespace VTX::UI::Widget::MainMenu::Tool
