#include "ui/old_ui/state/visualization.hpp"
#include "ui/old_ui/controller/freefly.hpp"
#include "ui/old_ui/controller/main_window_controller.hpp"
// #include "ui/old_ui/controller/measurement_picker.hpp"
#include "ui/old_ui/controller/picker.hpp"
#include "ui/old_ui/controller/trackball.hpp"
#include "ui/old_ui/style.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/define.hpp>
#include <app/component/render/camera.hpp>
#include <app/core/event/vtx_event.hpp>
#include <app/event.hpp>
#include <app/event/global.hpp>
#include <app/internal/scene/camera_manager.hpp>
#include <app/vtx_app.hpp>

namespace VTX
{
	namespace State
	{
		Visualization::Visualization()
		{
			_registerEvent( VTX::App::Event::Global::MOLECULE_ADDED );
			_registerEvent( VTX::App::Event::Global::MOLECULE_REMOVED );
			_registerEvent( VTX::App::Event::Global::MESH_ADDED );
			_registerEvent( VTX::App::Event::Global::MESH_REMOVED );

			// Create controller.
			_controllers.emplace( UI::ID::Controller::MAIN_WINDOW, new Controller::MainWindowController() );
			_controllers.emplace( UI::ID::Controller::FREEFLY,
								  new Controller::Freefly( App::VTXApp::get().getScene().getCameraManager() ) );
			_controllers.emplace( UI::ID::Controller::TRACKBALL,
								  new Controller::Trackball( App::VTXApp::get().getScene().getCameraManager(),
															 App::VTXApp::get().getScene().getAABB().centroid(),
															 App::VTXApp::get().getScene().getAABB().diameter() ) );
			_controllers.emplace( UI::ID::Controller::PICKER, new Controller::Picker() );
			//_controllers.emplace( ID::Controller::MEASUREMENT, new Controller::MeasurementPicker() );
		}

		void Visualization::enter( void * const p_arg )
		{
			BaseState::enter( p_arg );

			getController<Controller::Picker>( UI::ID::Controller::PICKER )->setActive( true );
			// getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT )->setActive( false );
			_pickerController = UI::ID::Controller::PICKER;

			if ( _cameraController == UI::ID::Controller::FREEFLY )
			{
				getController<Controller::Trackball>( UI::ID::Controller::TRACKBALL )->setActive( false );
			}
			else
			{
				getController<Controller::Freefly>( UI::ID::Controller::FREEFLY )->setActive( false );
			}
		}

		void Visualization::update( const float & p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			App::VTXApp::get().getScene().update( p_deltaTime );
			App::VTXApp::get().renderScene();
		}

		void Visualization::toggleCameraController()
		{
			if ( getController<VTX::Controller::BaseCameraController>( _cameraController )->isOrienting() )
			{
				return;
			}

			_controllers[ _cameraController ]->setActive( false );

			if ( _cameraController == UI::ID::Controller::FREEFLY )
			{
				_cameraController = UI::ID::Controller::TRACKBALL;
			}
			else
			{
				_cameraController = UI::ID::Controller::FREEFLY;
			}
			_controllers[ _cameraController ]->setActive( true );

			VTX_EVENT<const App::VTX_ID &>( VTX::App::Event::Global::CONTROLLER_CHANGE, _cameraController );
		}
		void Visualization::setCameraController( const App::VTX_ID & p_controllerId )
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

			VTX_EVENT<const App::VTX_ID &>( VTX::App::Event::Global::CONTROLLER_CHANGE, _cameraController );
		}

		void Visualization::resetCameraController()
		{
			// Reset camera.
			App::Component::Render::Camera & camera = App::VTXApp::get().getScene().getCamera();
			const Vec3f						 defaultPos
				= App::VTXApp::get().getScene().getAABB().centroid()
				  - App::Component::CAMERA_FRONT_DEFAULT * App::VTXApp::get().getScene().getAABB().radius()
						/ (float)( tan( Util::Math::radians( camera.getFov() ) * UI::Style::ORIENT_ZOOM_FACTOR ) );
			camera.reset( defaultPos );

			// Reset controllers.
			getController<Controller::Trackball>( UI::ID::Controller::TRACKBALL )->reset();
			getController<Controller::Freefly>( UI::ID::Controller::FREEFLY )->reset();
		}

		void Visualization::orientCameraController( const App::Component::Object3D::Helper::AABB & p_aabb )
		{
			getController<VTX::Controller::BaseCameraController>( _cameraController )->orient( p_aabb );

			// Override Trackball distance.
			if ( _cameraController == UI::ID::Controller::FREEFLY )
			{
				const Controller::Freefly * const freefly
					= getController<Controller::Freefly>( UI::ID::Controller::FREEFLY );
				if ( freefly->isOrienting() )
				{
					getController<Controller::Trackball>( UI::ID::Controller::TRACKBALL )
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
			if ( _cameraController == UI::ID::Controller::FREEFLY )
			{
				const Controller::Freefly * const freefly
					= getController<Controller::Freefly>( UI::ID::Controller::FREEFLY );
				if ( freefly->isOrienting() )
				{
					getController<Controller::Trackball>( UI::ID::Controller::TRACKBALL )
						->setDistanceForced( Util::Math::distance( p_position, freefly->getOrientTargetPosition() ) );
				}
			}
		}

		void Visualization::setPickerController( const App::VTX_ID & p_pickerId )
		{
			// Do nothing if id not in collection or already in use
			if ( _controllers.find( p_pickerId ) == _controllers.end() || _pickerController == p_pickerId )
				return;

			_controllers[ _pickerController ]->setActive( false );
			_pickerController = p_pickerId;
			_controllers[ _pickerController ]->setActive( true );

			VTX_EVENT( VTX::App::Event::Global::PICKER_MODE_CHANGE );
		}

		void Visualization::receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event )
		{
			// Recenter when add the first element in scene
			if ( p_event.name == VTX::App::Event::Global::MOLECULE_ADDED )
			{
				if ( App::VTXApp::get().getScene().getMolecules().size() == 1
					 && App::VTXApp::get().getScene().getMeshes().size() == 0 )
				{
					const VTX::App::Core::Event::VTXEventArg<App::Component::Chemistry::Molecule *> & castedEvent
						= dynamic_cast<
							const VTX::App::Core::Event::VTXEventArg<App::Component::Chemistry::Molecule *> &>(
							p_event );

					orientCameraController( castedEvent.get()->getWorldAABB() );
				}
			}
			else if ( p_event.name == VTX::App::Event::Global::MESH_ADDED )
			{
				if ( App::VTXApp::get().getScene().getMolecules().size() == 0
					 && App::VTXApp::get().getScene().getMeshes().size() == 1 )
					resetCameraController();
			}
		}

	} // namespace State
} // namespace VTX
