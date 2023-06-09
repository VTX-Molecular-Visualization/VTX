#include "visualization.hpp"
#include "controller/freefly.hpp"
#include "controller/main_window_controller.hpp"
#include "controller/measurement_picker.hpp"
#include "controller/picker.hpp"
#include "controller/trackball.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "model/molecule.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace State
	{
		Visualization::Visualization()
		{
			_registerEvent( Event::Global::MOLECULE_ADDED );
			_registerEvent( Event::Global::MOLECULE_REMOVED );
			_registerEvent( Event::Global::MESH_ADDED );
			_registerEvent( Event::Global::MESH_REMOVED );

			// Create controller.
			_controllers.emplace( ID::Controller::MAIN_WINDOW, new Controller::MainWindowController() );
			_controllers.emplace( ID::Controller::FREEFLY,
								  new Controller::Freefly( VTXApp::get().getScene().getCameraManager() ) );
			_controllers.emplace( ID::Controller::TRACKBALL,
								  new Controller::Trackball( VTXApp::get().getScene().getCameraManager(),
															 VTXApp::get().getScene().getAABB().centroid(),
															 VTXApp::get().getScene().getAABB().diameter() ) );
			_controllers.emplace( ID::Controller::PICKER, new Controller::Picker() );
			_controllers.emplace( ID::Controller::MEASUREMENT, new Controller::MeasurementPicker() );
		}

		void Visualization::enter( void * const p_arg )
		{
			BaseState::enter( p_arg );

			getController<Controller::Picker>( ID::Controller::PICKER )->setActive( true );
			getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT )->setActive( false );
			_pickerController = ID::Controller::PICKER;

			if ( _cameraController == ID::Controller::FREEFLY )
			{
				getController<Controller::Trackball>( ID::Controller::TRACKBALL )->setActive( false );
			}
			else
			{
				getController<Controller::Freefly>( ID::Controller::FREEFLY )->setActive( false );
			}
		}

		void Visualization::update( const float & p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			VTXApp::get().getScene().update( p_deltaTime );
			VTXApp::get().renderScene();
		}

		void Visualization::toggleCameraController()
		{
			if ( getController<VTX::Controller::BaseCameraController>( _cameraController )->isOrienting() )
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

			VTX_EVENT( new Event::VTXEventPtr( Event::Global::CONTROLLER_CHANGE, &_cameraController ) );
		}
		void Visualization::setCameraController( const ID::VTX_ID & p_controllerId )
		{
			if ( getController<VTX::Controller::BaseCameraController>( _cameraController )->isOrienting() )
			{
				return;
			}

			// Do nothing if id not in collection or already in use
			if ( _controllers.find( p_controllerId ) == _controllers.end() || _cameraController == p_controllerId )
				return;

			_controllers[ _cameraController ]->setActive( false );
			_cameraController = p_controllerId;
			_controllers[ _cameraController ]->setActive( true );

			VTX_EVENT( new Event::VTXEventPtr( Event::Global::CONTROLLER_CHANGE, &_cameraController ) );
		}

		void Visualization::resetCameraController()
		{
			// Reset camera.
			Object3D::Camera & camera = VTXApp::get().getScene().getCamera();
			const Vec3f		   defaultPos
				= VTXApp::get().getScene().getAABB().centroid()
				  - CAMERA_FRONT_DEFAULT * VTXApp::get().getScene().getAABB().radius()
						/ (float)( tan( Util::Math::radians( camera.getFov() ) * Style::ORIENT_ZOOM_FACTOR ) );
			camera.reset( defaultPos );

			// Reset controllers.
			getController<Controller::Trackball>( ID::Controller::TRACKBALL )->reset();
			getController<Controller::Freefly>( ID::Controller::FREEFLY )->reset();
		}

		void Visualization::orientCameraController( const Object3D::Helper::AABB & p_aabb )
		{
			getController<VTX::Controller::BaseCameraController>( _cameraController )->orient( p_aabb );

			// Override Trackball distance.
			if ( _cameraController == ID::Controller::FREEFLY )
			{
				const Controller::Freefly * const freefly
					= getController<Controller::Freefly>( ID::Controller::FREEFLY );
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
			getController<VTX::Controller::BaseCameraController>( _cameraController )
				->orient( p_position, p_orientation );

			// Override Trackball distance.
			if ( _cameraController == ID::Controller::FREEFLY )
			{
				const Controller::Freefly * const freefly
					= getController<Controller::Freefly>( ID::Controller::FREEFLY );
				if ( freefly->isOrienting() )
				{
					getController<Controller::Trackball>( ID::Controller::TRACKBALL )
						->setDistanceForced( Util::Math::distance( p_position, freefly->getOrientTargetPosition() ) );
				}
			}
		}

		void Visualization::setPickerController( const ID::VTX_ID & p_pickerId )
		{
			// Do nothing if id not in collection or already in use
			if ( _controllers.find( p_pickerId ) == _controllers.end() || _pickerController == p_pickerId )
				return;

			_controllers[ _pickerController ]->setActive( false );
			_pickerController = p_pickerId;
			_controllers[ _pickerController ]->setActive( true );

			VTX_EVENT( new Event::VTXEvent( Event::Global::PICKER_MODE_CHANGE ) );
		}

		void Visualization::receiveEvent( const Event::VTXEvent & p_event )
		{
			// Recenter when add the first element in scene
			if ( p_event.name == Event::MOLECULE_ADDED )
			{
				if ( VTXApp::get().getScene().getMolecules().size() == 1
					 && VTXApp::get().getScene().getMeshes().size() == 0 )
				{
					const Event::VTXEventPtr<Model::Molecule> & castedEvent
						= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );

					orientCameraController( castedEvent.ptr->getWorldAABB() );
				}
			}
			else if ( p_event.name == Event::MESH_ADDED )
			{
				if ( VTXApp::get().getScene().getMolecules().size() == 0
					 && VTXApp::get().getScene().getMeshes().size() == 1 )
					resetCameraController();
			}
		}

	} // namespace State
} // namespace VTX
