#include "tool/old_tool/controller/measurement_picker.hpp"
#include "tool/old_tool/action/measurement.hpp"
#include "tool/old_tool/model/measurement/measure_in_progress.hpp"
#include <QPoint>
#include <app/core/action/action_manager.hpp>
#include <app/action/selection.hpp>
#include <app/core/event/event_manager.hpp>
#include <app/old_app/model/atom.hpp>
#include <app/old_app/model/molecule.hpp>
#include <app/old_app/model/residue.hpp>
#include <app/old_app/model/selection.hpp>
#include <app/core/mvc/mvc_manager.hpp>
#include <app/old_app/selection/selection_manager.hpp>
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
		_currentMeasureModel = VTX::Core::MVC::MvcManager::get().instantiateModel<Model::Measurement::MeasureInProgress>();
	}
	MeasurementPicker::~MeasurementPicker() { VTX::Core::MVC::MvcManager::get().deleteModel( _currentMeasureModel ); }

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
		UI::MainWindow &						 mw = VTXApp::get().getMainWindow();
		const UI::Widget::Render::RenderWidget & renderWidget
			= mw.getWidget<UI::Widget::Render::RenderWidget>( ID::UI::Window::RENDER );

		const QPoint position = renderWidget.mapToGlobal( QPoint( p_x, p_y ) );

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

		const Vec2i ids = VTXApp::get().getMainWindow().getPickedIds( p_event.pos().x(), p_event.pos().y() );

		if ( ids.x != Model::ID_UNKNOWN )
		{
			if ( ids.y != Model::ID_UNKNOWN )
			{
				// Bond clicked => set atom pair to next target
				if ( _currentMode == Mode::DISTANCE && _currentMeasureModel->getAtomCount() == 0 )
				{
					const Model::Atom & firstAtom  = VTX::Core::MVC::MvcManager::get().getModel<Model::Atom>( ids.x );
					const Model::Atom & secondAtom = VTX::Core::MVC::MvcManager::get().getModel<Model::Atom>( ids.y );

					_currentMeasureModel->setPotentialNextTarget( firstAtom, secondAtom );
					hasFindTarget = true;
				}
			}
			else
			{
				const ID::VTX_ID & typeId = VTX::Core::MVC::MvcManager::get().getModelTypeID( ids.x );
				Model::ID		   atomID;

				// If residue => select alpha carbon
				if ( typeId == ID::Model::MODEL_RESIDUE )
				{
					const Model::Residue & residue = VTX::Core::MVC::MvcManager::get().getModel<Model::Residue>( ids.x );
					atomID						   = residue.getAlphaCarbon()->getId();
				}
				else // => Atom
				{
					atomID = ids.x;
				}

				// Atom picked
				const Model::Atom & atom = VTX::Core::MVC::MvcManager::get().getModel<Model::Atom>( atomID );
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
					UI::VTXApp::get()
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

			VTX_EVENT( new VTX::Event::VTXEvent( VTX::Event::Global::PICKER_MODE_CHANGE ) );
		}
	}
} // namespace VTX::Controller
