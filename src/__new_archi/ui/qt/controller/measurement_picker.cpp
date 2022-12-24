#include "measurement_picker.hpp"
#include "__new_archi/ui/qt/application_qt.hpp"
#include "__new_archi/ui/qt/state/state_machine.hpp"
#include "__new_archi/ui/qt/state/visualization.hpp"
#include "action/action_manager.hpp"
#include "action/measurement.hpp"
#include "action/selection.hpp"
#include "event/event_manager.hpp"
#include "model/atom.hpp"
#include "model/measurement/measure_in_progress.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "tool/logger.hpp"
#include "ui/cursor_handler.hpp"
#include <QPoint>
#include <util/math.hpp>

namespace VTX::UI::QT::Controller
{
	MeasurementPicker::MeasurementPicker()
	{
		_currentMeasureModel = MVC::MvcManager::get().instantiateModel<Model::Measurement::MeasureInProgress>();
	}
	MeasurementPicker::~MeasurementPicker() { MVC::MvcManager::get().deleteModel( _currentMeasureModel ); }

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
		const QPoint position = QT_APP()->getMainWindow().getRender()->mapToGlobal( QPoint( p_x, p_y ) );

		Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

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

		const Vec2i ids = QT_APP()->getMainWindow().getRender()->getPickedIds( p_event.pos().x(), p_event.pos().y() );

		if ( ids.x != Model::ID_UNKNOWN )
		{
			if ( ids.y != Model::ID_UNKNOWN )
			{
				// Bond clicked => set atom pair to next target
				if ( _currentMode == Mode::DISTANCE && _currentMeasureModel->getAtomCount() == 0 )
				{
					const Model::Atom & firstAtom  = MVC::MvcManager::get().getModel<Model::Atom>( ids.x );
					const Model::Atom & secondAtom = MVC::MvcManager::get().getModel<Model::Atom>( ids.y );

					_currentMeasureModel->setPotentialNextTarget( firstAtom, secondAtom );
					hasFindTarget = true;
				}
			}
			else
			{
				const ID::VTX_ID & typeId = MVC::MvcManager::get().getModelTypeID( ids.x );
				Model::ID		   atomID;

				// If residue => select alpha carbon
				if ( typeId == ID::Model::MODEL_RESIDUE )
				{
					const Model::Residue & residue = MVC::MvcManager::get().getModel<Model::Residue>( ids.x );
					atomID						   = residue.getAlphaCarbon()->getId();
				}
				else // => Atom
				{
					atomID = ids.x;
				}

				// Atom picked
				const Model::Atom & atom = MVC::MvcManager::get().getModel<Model::Atom>( atomID );
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

			// VTXApp::get()
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
					VTXApp::get()
						.getStateMachine()
						.getState<State::Visualization>( ID::State::VISUALIZATION )
						->setPickerController( ID::Controller::PICKER );
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
			const Model::Atom * const firstAtom	 = _currentMeasureModel->getAtom( 0 );
			const Model::Atom * const secondAtom = _currentMeasureModel->getAtom( 1 );

			VTX_ACTION( new Action::Measurement::InstantiateDistanceLabel( *firstAtom, *secondAtom ) );
		}
		break;

		case Mode::ANGLE:
		{
			const Model::Atom * const firstAtom	 = _currentMeasureModel->getAtom( 0 );
			const Model::Atom * const secondAtom = _currentMeasureModel->getAtom( 1 );
			const Model::Atom * const thirdAtom	 = _currentMeasureModel->getAtom( 2 );

			VTX_ACTION( new Action::Measurement::InstantiateAngleLabel( *firstAtom, *secondAtom, *thirdAtom ) );
		}
		break;

		case Mode::DIHEDRAL_ANGLE:
		{
			const Model::Atom * const firstAtom	 = _currentMeasureModel->getAtom( 0 );
			const Model::Atom * const secondAtom = _currentMeasureModel->getAtom( 1 );
			const Model::Atom * const thirdAtom	 = _currentMeasureModel->getAtom( 2 );
			const Model::Atom * const fourthAtom = _currentMeasureModel->getAtom( 3 );

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

			VTX_EVENT( new Event::VTXEvent( Event::Global::PICKER_MODE_CHANGE ) );
		}
	}
} // namespace VTX::UI::QT::Controller
