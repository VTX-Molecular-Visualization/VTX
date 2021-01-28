#include "visualization.hpp"
#include "controller/freefly.hpp"
#include "controller/shortcut.hpp"
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
		}

		void Visualization::enter( void * const )
		{
			// Create controller.
			addItem( ID::Controller::FREEFLY, new Controller::Freefly( VTXApp::get().getScene().getCamera() ) );
			addItem( ID::Controller::TRACKBALL,
					 new Controller::Trackball( VTXApp::get().getScene().getCamera(),
												VTXApp::get().getScene().getAABB().centroid(),
												VTXApp::get().getScene().getAABB().diameter() ) );
			addItem( ID::Controller::SHORTCUT, new Controller::Shortcut() );

			if ( _cameraController == ID::Controller::FREEFLY )
			{
				getItem<Controller::Trackball>( ID::Controller::TRACKBALL )->setActive( false );
			}
			else
			{
				getItem<Controller::Freefly>( ID::Controller::FREEFLY )->setActive( false );
			}
		}

		void Visualization::exit() { Generic::HasCollection<Controller::BaseController>::clear(); }

		void Visualization::update( const float & p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			VTXApp::get().getScene().update( p_deltaTime );

			if ( VTX_SETTING().activeRenderer )
			{
				VTXApp::get().renderScene();
			}
		}

		void Visualization::toggleCameraController()
		{
			if ( getItem<VTX::Controller::BaseCameraController>( _cameraController )->isOrienting() )
			{
				return;
			}

			getItem( _cameraController )->setActive( false );

			if ( _cameraController == ID::Controller::FREEFLY )
			{
				_cameraController = ID::Controller::TRACKBALL;
			}
			else
			{
				_cameraController = ID::Controller::FREEFLY;
			}
			getItem( _cameraController )->setActive( true );

			VTX_EVENT( new Event::VTXEventPtr( Event::Global::CONTROLLER_CHANGE, &_cameraController ) );
		}
		void Visualization::setCameraController( const ID::VTX_ID & p_controllerId )
		{
			if ( getItem<VTX::Controller::BaseCameraController>( _cameraController )->isOrienting() )
			{
				return;
			}

			// Do nothing if id not in collection or already in use
			if ( !hasItem( p_controllerId ) || _cameraController == p_controllerId )
				return;

			getItem( _cameraController )->setActive( false );
			_cameraController = p_controllerId;
			getItem( _cameraController )->setActive( true );

			VTX_EVENT( new Event::VTXEventPtr( Event::Global::CONTROLLER_CHANGE, &_cameraController ) );
		}

		void Visualization::resetCameraController()
		{
			if ( getItem<VTX::Controller::BaseCameraController>( _cameraController )->isOrienting() )
			{
				return;
			}
			getItem<Controller::Trackball>( ID::Controller::TRACKBALL )->reset();
			getItem<Controller::Freefly>( ID::Controller::FREEFLY )->reset();
		}

		void Visualization::orientCameraController( const Math::AABB & p_aabb )
		{
			getItem<VTX::Controller::BaseCameraController>( _cameraController )->orient( p_aabb );
			// Override Trackball distance.
			if ( _cameraController == ID::Controller::FREEFLY )
			{
				const Controller::Freefly * const freefly = getItem<Controller::Freefly>( ID::Controller::FREEFLY );
				if ( freefly->isOrienting() )
				{
					getItem<Controller::Trackball>( ID::Controller::TRACKBALL )
						->setDistanceForced(
							Util::Math::distance( p_aabb.centroid(), freefly->getOrientTargetPosition() ) );
				}
			}
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

					orientCameraController( castedEvent.ptr->getAABB() );
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
