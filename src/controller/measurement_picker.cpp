#include "measurement_picker.hpp"
#include "action/action_manager.hpp"
#include "action/measurement.hpp"
#include "action/selection.hpp"
#include "event/event_manager.hpp"
#include "model/atom.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "tool/logger.hpp"
#include "ui/cursor_handler.hpp"
#include "ui/main_window.hpp"
#include "util/math.hpp"
#include "vtx_app.hpp"

namespace VTX::Controller
{
	MeasurementPicker::MeasurementPicker() : _lastClickedIds { Model::ID_UNKNOWN, Model::ID_UNKNOWN } {}
	void MeasurementPicker::update( const float & p_deltaTime ) { BaseMouseController::update( p_deltaTime ); }

	void MeasurementPicker::_onMouseLeftClick( const uint p_x, const uint p_y )
	{
		const Vec2i ids			  = VTXApp::get().getMainWindow().getOpenGLWidget().getPickedIds( p_x, p_y );
		bool		updateDisplay = false;

		// If something clicked.
		if ( ids.x != Model::ID_UNKNOWN )
		{
			const ID::VTX_ID & typeId = MVC::MvcManager::get().getModelTypeID( ids.x );

			if ( ids.y != Model::ID_UNKNOWN ) // Bond
			{
				if ( _currentMode == Mode::DISTANCE )
				{
					atoms.clear();
					atoms.emplace_back( ids.x );
					atoms.emplace_back( ids.y );

					updateDisplay = true;
				}
			}
			else
			{
				Model::ID atomID;

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

				if ( std::find( atoms.cbegin(), atoms.cend(), atomID ) == atoms.cend() )
				{
					atoms.emplace_back( atomID );
					updateDisplay = true;
				}
			}
		}
		else
		{
			if ( atoms.size() > 0 )
			{
				atoms.clear();
				updateDisplay = true;
			}
		}

		if ( updateDisplay )
			_updateDisplay();

		_lastClickedIds = ids;
	}

	void MeasurementPicker::_onMouseRightClick( const uint p_x, const uint p_y )
	{
		UI::Widget::Render::OpenGLWidget & openGLWidget = VTXApp::get().getMainWindow().getOpenGLWidget();
		_lastClickedIds									= openGLWidget.getPickedIds( p_x, p_y );

		Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();

		const QPoint position = openGLWidget.mapToGlobal( QPoint( p_x, p_y ) );

		if ( selection.isEmpty() )
		{
			UI::ContextualMenu::pop( UI::ContextualMenu::Menu::Render, position );
		}
		else
		{
			UI::ContextualMenu::pop( UI::ContextualMenu::Menu::Selection, &selection, position );
		}
	}

	void MeasurementPicker::_onMouseLeftDoubleClick( const uint p_x, const uint p_y )
	{
		const Vec2i ids = VTXApp::get().getMainWindow().getOpenGLWidget().getPickedIds( p_x, p_y );

		// If double click on void => reset to selection picker
		if ( ids.x == Model::ID_UNKNOWN )
		{
			VTXApp::get()
				.getStateMachine()
				.getState<State::Visualization>( ID::State::VISUALIZATION )
				->setPickerController( ID::Controller::PICKER );
		}
	}

	void MeasurementPicker::_updateDisplay()
	{
		bool hasInstantiatedLabel = false;
		switch ( _currentMode )
		{
		case Mode::DISTANCE:
			if ( atoms.size() == 2 )
			{
				const Model::Atom & firstAtom  = MVC::MvcManager::get().getModel<Model::Atom>( atoms[ 0 ] );
				const Model::Atom & secondAtom = MVC::MvcManager::get().getModel<Model::Atom>( atoms[ 1 ] );

				VTX_ACTION( new Action::Measurement::InstantiateDistanceLabel( firstAtom, secondAtom ) );
				hasInstantiatedLabel = true;
			}
			break;

		case Mode::ANGLE:
			if ( atoms.size() == 3 )
			{
				const Model::Atom & firstAtom  = MVC::MvcManager::get().getModel<Model::Atom>( atoms[ 0 ] );
				const Model::Atom & secondAtom = MVC::MvcManager::get().getModel<Model::Atom>( atoms[ 1 ] );
				const Model::Atom & thirdAtom  = MVC::MvcManager::get().getModel<Model::Atom>( atoms[ 2 ] );

				VTX_ACTION( new Action::Measurement::InstantiateAngleLabel( firstAtom, secondAtom, thirdAtom ) );
				hasInstantiatedLabel = true;
			}
			break;

		case Mode::DIHEDRAL_ANGLE:
			if ( atoms.size() == 4 )
			{
				const Model::Atom & firstAtom  = MVC::MvcManager::get().getModel<Model::Atom>( atoms[ 0 ] );
				const Model::Atom & secondAtom = MVC::MvcManager::get().getModel<Model::Atom>( atoms[ 1 ] );
				const Model::Atom & thirdAtom  = MVC::MvcManager::get().getModel<Model::Atom>( atoms[ 2 ] );
				const Model::Atom & fourthAtom = MVC::MvcManager::get().getModel<Model::Atom>( atoms[ 3 ] );

				VTX_ACTION( new Action::Measurement::InstantiateDihedralAngleLabel(
					firstAtom, secondAtom, thirdAtom, fourthAtom ) );
				hasInstantiatedLabel = true;
			}
			break;
		}

		if ( hasInstantiatedLabel )
			atoms.clear();
	}

	void MeasurementPicker::setCurrentMode( const Mode & p_mode )
	{
		if ( _currentMode != p_mode )
		{
			atoms.clear();
			_currentMode = p_mode;

			VTX_EVENT( new Event::VTXEvent( Event::Global::PICKER_MODE_CHANGE ) );
		}
	}
} // namespace VTX::Controller
