#include "ui/qt/state/visualization.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/controller/freefly.hpp"
#include "ui/qt/controller/main_window_controller.hpp"
#include "ui/qt/controller/measurement_picker.hpp"
#include "ui/qt/controller/picker.hpp"
#include "ui/qt/controller/trackball.hpp"
#include <app/old/application/scene.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/component/define.hpp>
#include <app/old/component/render/camera.hpp>
#include <app/old/core/event/vtx_event.hpp>
#include <app/old/event.hpp>
#include <app/old/event/global.hpp>
#include <app/old/vtx_app.hpp>
#include <util/math.hpp>

namespace VTX::UI::QT::State
{
	Visualization::Visualization()
	{
		_registerEvent( VTX::App::Old::Event::Global::MOLECULE_ADDED );
		_registerEvent( VTX::App::Old::Event::Global::MOLECULE_REMOVED );
		_registerEvent( VTX::App::Old::Event::Global::MESH_ADDED );
		_registerEvent( VTX::App::Old::Event::Global::MESH_REMOVED );

		// Create controller.
		_controllers.emplace( ID::Controller::MAIN_WINDOW, new QT::Controller::MainWindowController() );
		_controllers.emplace( ID::Controller::FREEFLY,
							  new QT::Controller::Freefly( App::Old::VTXApp::get().getScene().getCamera() ) );
		_controllers.emplace( ID::Controller::TRACKBALL,
							  new QT::Controller::Trackball( App::Old::VTXApp::get().getScene().getCamera(),
															 App::Old::VTXApp::get().getScene().getAABB().centroid(),
															 App::Old::VTXApp::get().getScene().getAABB().diameter() ) );
		_controllers.emplace( ID::Controller::PICKER, new QT::Controller::Picker() );
		_controllers.emplace( ID::Controller::MEASUREMENT, new QT::Controller::MeasurementPicker() );
	}

	void Visualization::enter( void * const p_arg )
	{
		BaseState::enter( p_arg );

		getController<QT::Controller::Picker>( ID::Controller::PICKER )->setActive( true );
		getController<QT::Controller::MeasurementPicker>( ID::Controller::MEASUREMENT )->setActive( false );
		_pickerController = ID::Controller::PICKER;

		if ( _cameraController == ID::Controller::FREEFLY )
		{
			getController<QT::Controller::Trackball>( ID::Controller::TRACKBALL )->setActive( false );
		}
		else
		{
			getController<QT::Controller::Freefly>( ID::Controller::FREEFLY )->setActive( false );
		}
	}

	void Visualization::update( const float & p_deltaTime )
	{
		BaseState::update( p_deltaTime );

		App::Old::VTXApp::get().getScene().update( p_deltaTime );
		QT_APP()->renderScene();
	}

	void Visualization::toggleCameraController()
	{
		if ( getController<QT::Controller::BaseCameraController>( _cameraController )->isOrienting() )
		{
			return;
		}

		_controllers[ _cameraController ]->setActive( false );

		if ( _cameraController == ID::Controller::FREEFLY )
		{
			_cameraController = ID::Controller::TRACKBALL;
		}
		else
		{
			_cameraController = ID::Controller::FREEFLY;
		}
		_controllers[ _cameraController ]->setActive( true );

		VTX_EVENT<const App::Old::VTX_ID &>( VTX::App::Old::Event::Global::CONTROLLER_CHANGE, _cameraController );
	}
	void Visualization::setCameraController( const App::Old::VTX_ID & p_controllerId )
	{
		if ( getController<QT::Controller::BaseCameraController>( _cameraController )->isOrienting() )
		{
			return;
		}

		// Do nothing if id not in collection or already in use
		if ( _controllers.find( p_controllerId ) == _controllers.end() || _cameraController == p_controllerId )
			return;

		_controllers[ _cameraController ]->setActive( false );
		_cameraController = p_controllerId;
		_controllers[ _cameraController ]->setActive( true );

		VTX_EVENT<const App::Old::VTX_ID &>( VTX::App::Old::Event::Global::CONTROLLER_CHANGE, _cameraController );
	}

	void Visualization::resetCameraController()
	{
		// Reset camera.
		App::Old::Component::Render::Camera & camera = App::Old::VTXApp::get().getScene().getCamera();
		const Vec3f						 defaultPos
			= App::Old::VTXApp::get().getScene().getAABB().centroid()
			  - App::Old::Component::CAMERA_FRONT_DEFAULT * App::Old::VTXApp::get().getScene().getAABB().radius()
					/ (float)( tan( Util::Math::radians( camera.getFov() ) * UI::Style::ORIENT_ZOOM_FACTOR ) );
		camera.reset( defaultPos );

		// Reset controllers.
		getController<QT::Controller::Trackball>( ID::Controller::TRACKBALL )->reset();
		getController<QT::Controller::Freefly>( ID::Controller::FREEFLY )->reset();
	}

	void Visualization::orientCameraController( const App::Old::Component::Object3D::Helper::AABB & p_aabb )
	{
		getController<QT::Controller::BaseCameraController>( _cameraController )->orient( p_aabb );
		// Override Trackball distance.
		if ( _cameraController == ID::Controller::FREEFLY )
		{
			const Controller::Freefly * const freefly = getController<Controller::Freefly>( ID::Controller::FREEFLY );
			if ( freefly->isOrienting() )
			{
				getController<Controller::Trackball>( ID::Controller::TRACKBALL )
					->setDistanceForced(
						Util::Math::distance( p_aabb.centroid(), freefly->getOrientTargetPosition() ) );
			}
		}
	}
	void Visualization::orientCameraController( const Vec3f & p_position, const Quatf & p_orientation )
	{
		getController<QT::Controller::BaseCameraController>( _cameraController )->orient( p_position, p_orientation );

		// Override Trackball distance.
		if ( _cameraController == ID::Controller::FREEFLY )
		{
			const Controller::Freefly * const freefly = getController<Controller::Freefly>( ID::Controller::FREEFLY );
			if ( freefly->isOrienting() )
			{
				getController<Controller::Trackball>( ID::Controller::TRACKBALL )
					->setDistanceForced( Util::Math::distance( p_position, freefly->getOrientTargetPosition() ) );
			}
		}
	}

	void Visualization::setPickerController( const App::Old::VTX_ID & p_pickerId )
	{
		// Do nothing if id not in collection or already in use
		if ( _controllers.find( p_pickerId ) == _controllers.end() || _pickerController == p_pickerId )
			return;

		_controllers[ _pickerController ]->setActive( false );
		_pickerController = p_pickerId;
		_controllers[ _pickerController ]->setActive( true );

		VTX_EVENT( VTX::App::Old::Event::Global::PICKER_MODE_CHANGE );
	}

	void Visualization::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event )
	{
		// Recenter when add the first element in scene
		if ( p_event.name == VTX::App::Old::Event::Global::MOLECULE_ADDED )
		{
			if ( App::Old::VTXApp::get().getScene().getMolecules().size() == 1
				 && App::Old::VTXApp::get().getScene().getMeshes().size() == 0 )
			{
				const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Molecule *> & castedEvent
					= dynamic_cast<const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Component::Chemistry::Molecule *> &>(
						p_event );

				orientCameraController( castedEvent.get()->getAABB() );
			}
		}
		else if ( p_event.name == VTX::App::Old::Event::Global::MESH_ADDED )
		{
			if ( App::Old::VTXApp::get().getScene().getMolecules().size() == 0
				 && App::Old::VTXApp::get().getScene().getMeshes().size() == 1 )
				resetCameraController();
		}
	}

} // namespace VTX::UI::QT::State