#include "tool/old_tool/controller/measurement_picker.hpp"
#include "tool/old_tool/action/measurement.hpp"
#include "tool/old_tool/model/measurement/measure_in_progress.hpp"
#include <QPoint>
#include <app/old/action/selection.hpp>
#include <app/old/application/selection/selection.hpp>
#include <app/old/application/selection/selection_manager.hpp>
#include <app/old/component/chemistry/atom.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/component/chemistry/residue.hpp>
#include <app/old/event.hpp>
#include <app/old/event/global.hpp>
#include <app/old/mvc.hpp>
#include <ui/old_ui/state/state_machine.hpp>
#include <ui/old_ui/state/visualization.hpp>
#include <ui/old_ui/ui/cursor_handler.hpp>
#include <ui/old_ui/ui/main_window.hpp>
#include <ui/old_ui/vtx_app.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::Controller
{
	MeasurementPicker::MeasurementPicker()
	{
		_currentMeasureModel = VTX::MVC_MANAGER().instantiateModel<Model::Measurement::MeasureInProgress>();
	}
	MeasurementPicker::~MeasurementPicker() { VTX::MVC_MANAGER().deleteModel( _currentMeasureModel ); }

	void MeasurementPicker::_onMouseLeftClick( const uint p_x, const uint p_y )
	{
		if ( _currentMeasureModel->applyPotentialTarget() && _canCreateLabel() )
		{
			_createLabel();
		}
	}

	void MeasurementPicker::update( const float & p_deltaTime ) { BaseMouseController::update( p_deltaTime ); }

	void MeasurementPicker::_onMouseRightClick( const uint p_x, const uint p_y )
	{
		UI::MainWindow &						 mw = UI::VTXApp::get().getMainWindow();
		const UI::Widget::Render::RenderWidget & renderWidget
			= mw.getWidget<UI::Widget::Render::RenderWidget>( UI::ID::Window::RENDER );

		const QPoint position = renderWidget.mapToGlobal( QPoint( p_x, p_y ) );

		App::Old::Application::Selection::SelectionModel & selection
			= VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel();

		if ( selection.isEmpty() )
		{
			UI::ContextualMenu::pop( UI::ContextualMenu::Menu::Render, position );
		}
		else
		{
			UI::ContextualMenu::pop( UI::ContextualMenu::Menu::Selection, &selection, position );
		}
	}

	void MeasurementPicker::_handleMouseMotionEvent( const QMouseEvent & p_event )
	{
		BaseMouseController::_handleMouseMotionEvent( p_event );

		// If the user is doing an action, we hide the potential next measure
		if ( _mouseLeftPressed || _mouseMiddlePressed || _mouseRightPressed )
		{
			if ( _currentMeasureModel->getPotentialNextTargetType()
				 != Model::Measurement::MeasureInProgress::PotentialTargetType::NONE )
			{
				_currentMeasureModel->clearPotentialTarget();
			}

			return;
		}

		bool hasFindTarget = false;

		const Vec2i ids = UI::VTXApp::get().getMainWindow().getPickedIds( p_event.pos().x(), p_event.pos().y() );

		if ( ids.x != App::Old::Core::Model::ID_UNKNOWN )
		{
			if ( ids.y != App::Old::Core::Model::ID_UNKNOWN )
			{
				// Bond clicked => set atom pair to next target
				if ( _currentMode == Mode::DISTANCE && _currentMeasureModel->getAtomCount() == 0 )
				{
					const App::Old::Component::Chemistry::Atom & firstAtom
						= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Atom>( ids.x );
					const App::Old::Component::Chemistry::Atom & secondAtom
						= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Atom>( ids.y );

					_currentMeasureModel->setPotentialNextTarget( firstAtom, secondAtom );
					hasFindTarget = true;
				}
			}
			else
			{
				const App::Old::VTX_ID &	 typeId = VTX::MVC_MANAGER().getModelTypeID( ids.x );
				App::Old::Core::Model::ID atomID;

				// If residue => select alpha carbon
				if ( typeId == App::Old::ID::Model::MODEL_RESIDUE )
				{
					const App::Old::Component::Chemistry::Residue & residue
						= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Residue>( ids.x );
					atomID = residue.getAlphaCarbon()->getId();
				}
				else // => Atom
				{
					atomID = ids.x;
				}

				// Atom picked
				const App::Old::Component::Chemistry::Atom & atom
					= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Atom>( atomID );
				if ( !_currentMeasureModel->contains( atom ) )
				{
					_currentMeasureModel->setPotentialNextTarget( atom );
					hasFindTarget = true;
				}
			}
		}

		if ( !hasFindTarget )
		{
			_currentMeasureModel->setPotentialNextTarget( p_event.globalPos() );
		}
	}

	void MeasurementPicker::_onMouseLeftDoubleClick( const uint p_x, const uint p_y )
	{
		const Model::Measurement::MeasureInProgress::PotentialTargetType currentTargetType
			= _currentMeasureModel->getPotentialNextTargetType();

		// If double click on void => reset to selection picker
		if ( currentTargetType == Model::Measurement::MeasureInProgress::PotentialTargetType::NONE
			 || currentTargetType == Model::Measurement::MeasureInProgress::PotentialTargetType::POSITION )
		{
			_currentMeasureModel->clearAtoms();

			// App::Old::VTXApp::get()
			//	.getStateMachine()
			//	.getState<State::Visualization>( ID::State::VISUALIZATION )
			//	->setPickerController( ID::Controller::PICKER );
		}
	}

	void MeasurementPicker::receiveEvent( const QKeyEvent & p_event )
	{
		if ( p_event.key() == Qt::Key::Key_Escape )
		{
			const Model::Measurement::MeasureInProgress::PotentialTargetType currentTargetType
				= _currentMeasureModel->getPotentialNextTargetType();

			if ( currentTargetType == Model::Measurement::MeasureInProgress::PotentialTargetType::NONE
				 || currentTargetType == Model::Measurement::MeasureInProgress::PotentialTargetType::POSITION )
			{
				if ( _currentMeasureModel->getAtomCount() > 0 )
				{
					_currentMeasureModel->clearAtoms();
				}
				else
				{
					UI::VTXApp::get()
						.getStateMachine()
						.getState<State::Visualization>( UI::ID::State::VISUALIZATION )
						->setPickerController( UI::ID::Controller::PICKER );
				}
			}
		}
	}

	bool MeasurementPicker::_canCreateLabel() const
	{
		bool res = false;

		switch ( _currentMode )
		{
		case Mode::DISTANCE: res = _currentMeasureModel->getAtomCount() == 2; break;
		case Mode::ANGLE: res = _currentMeasureModel->getAtomCount() == 3; break;
		case Mode::DIHEDRAL_ANGLE: res = _currentMeasureModel->getAtomCount() == 4; break;
		}

		return res;
	}
	void MeasurementPicker::_createLabel()
	{
		switch ( _currentMode )
		{
		case Mode::DISTANCE:
		{
			const App::Old::Component::Chemistry::Atom * const firstAtom	 = _currentMeasureModel->getAtom( 0 );
			const App::Old::Component::Chemistry::Atom * const secondAtom = _currentMeasureModel->getAtom( 1 );

			VTX_ACTION( new Action::Measurement::InstantiateDistanceLabel( *firstAtom, *secondAtom ) );
		}
		break;

		case Mode::ANGLE:
		{
			const App::Old::Component::Chemistry::Atom * const firstAtom	 = _currentMeasureModel->getAtom( 0 );
			const App::Old::Component::Chemistry::Atom * const secondAtom = _currentMeasureModel->getAtom( 1 );
			const App::Old::Component::Chemistry::Atom * const thirdAtom	 = _currentMeasureModel->getAtom( 2 );

			VTX_ACTION( new Action::Measurement::InstantiateAngleLabel( *firstAtom, *secondAtom, *thirdAtom ) );
		}
		break;

		case Mode::DIHEDRAL_ANGLE:
		{
			const App::Old::Component::Chemistry::Atom * const firstAtom	 = _currentMeasureModel->getAtom( 0 );
			const App::Old::Component::Chemistry::Atom * const secondAtom = _currentMeasureModel->getAtom( 1 );
			const App::Old::Component::Chemistry::Atom * const thirdAtom	 = _currentMeasureModel->getAtom( 2 );
			const App::Old::Component::Chemistry::Atom * const fourthAtom = _currentMeasureModel->getAtom( 3 );

			VTX_ACTION( new Action::Measurement::InstantiateDihedralAngleLabel(
				*firstAtom, *secondAtom, *thirdAtom, *fourthAtom ) );
		}
		break;
		}

		_currentMeasureModel->clearAtoms();
	}

	void MeasurementPicker::setCurrentMode( const Mode & p_mode )
	{
		if ( _currentMode != p_mode )
		{
			_currentMeasureModel->clearAtoms();
			_currentMode = p_mode;

			VTX_EVENT( VTX::App::Old::Event::Global::PICKER_MODE_CHANGE );
		}
	}
} // namespace VTX::Controller
