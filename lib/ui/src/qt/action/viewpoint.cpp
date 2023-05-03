#include "ui/qt/action/viewpoint.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/controller/freefly.hpp"
#include "ui/qt/controller/trackball.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/visualization.hpp"
#include "ui/qt/style.hpp"
#include <app/application/scene.hpp>
#include <app/application/selection/selection.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/mvc.hpp>
#include <app/old_app/vtx_app.hpp>
#include <set>

namespace VTX::UI::QT::Action::Viewpoint
{
	Create::Create() :
		Create( *VTXApp::get().getScene().getPaths()[ 0 ],
				VTXApp::get().getScene().getCamera(),
				QT_APP()
					->getStateMachine()
					.getState<State::Visualization>( ID::State::VISUALIZATION )
					->getCurrentCameraController() )
	{
	}

	Create::Create( App::Component::Video::Path &			 p_path,
					const App::Component::Render::Camera &	 p_camera,
					Controller::BaseCameraController * const p_controller ) :
		_path( p_path ),
		_rotation( p_camera.getRotation() ), _position( p_camera.getPosition() ), _controller( p_controller->getID() )
	{
		_tag = VTX::App::Core::Action::ACTION_TAG( _tag | VTX::App::Core::Action::ACTION_TAG::MODIFY_SCENE );

		const State::Visualization * const visualizationState
			= QT_APP()->getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		const Controller::Trackball * const trackball
			= visualizationState->getController<Controller::Trackball>( ID::Controller::TRACKBALL );

		if ( visualizationState->getCurrentCameraControllerID() == ID::Controller::TRACKBALL )
		{
			_target = trackball->getTarget();
		}
		else
		{
			_target = trackball->targetSimulationFromCamera( p_camera );
		}
	}

	std::string Create::_generateViewpointName() const
	{
		int intSuffix = 1;

		std::string viewpointName;
		bool		nameIsValid;
		do
		{
			viewpointName = Style::VIEWPOINT_DEFAULT_NAME + ' ' + std::to_string( intSuffix );
			nameIsValid	  = true;

			for ( const App::Component::Object3D::Viewpoint * const viewpoint : _path.getViewpoints() )
			{
				if ( viewpoint->getDefaultName() == viewpointName )
				{
					nameIsValid = false;
					break;
				}
			}

			intSuffix++;

		} while ( !nameIsValid );

		return viewpointName;
	}

	void Create::execute()
	{
		const std::string							viewpointName = _generateViewpointName();
		App::Component::Object3D::Viewpoint * const viewpoint
			= VTX::MVC_MANAGER()
				  .instantiateModel<App::Component::Object3D::Viewpoint, App::Component::Video::Path * const>( &_path );
		viewpoint->setController( _controller );
		viewpoint->setRotation( _rotation );
		viewpoint->setPosition( _position );

		viewpoint->setName( viewpointName );

		viewpoint->setTarget( _target );
		viewpoint->setDistance( VTX::Util::Math::distance( _position, _target ) );

		_path.addViewpoint( viewpoint );
		_path.refreshAllDurations();
	}

	void Delete::execute()
	{
		VTX::App::Application::Selection::SelectionManager::get().getSelectionModel().unselectModels( _viewpoints );

		std::set<App::Component::Video::Path *> paths = std::set<App::Component::Video::Path *>();
		for ( App::Component::Object3D::Viewpoint * const viewpoint : _viewpoints )
		{
			App::Component::Video::Path * const path = viewpoint->getPathPtr();
			path->removeViewpoint( viewpoint );
			paths.emplace( path );

			VTX::MVC_MANAGER().deleteModel( viewpoint );
		}

		for ( App::Component::Video::Path * const path : paths )
			path->refreshAllDurations();
	}

	GoTo::GoTo( const App::Component::Object3D::Viewpoint & p_viewpoint ) :
		GoTo( p_viewpoint, VTXApp::get().getScene().getCamera() )
	{
	}
	void GoTo::execute()
	{
		State::Visualization * const state
			= QT_APP()->getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		if ( state->getCurrentCameraControllerID() == ID::Controller::TRACKBALL )
		{
			Controller::Trackball * const trackball
				= dynamic_cast<Controller::Trackball * const>( state->getCurrentCameraController() );
			trackball->setTarget( _viewpoint.getTarget() );
		}

		QT_APP()
			->getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->orientCameraController( _viewpoint.getPosition(), _viewpoint.getRotation() );
	}
} // namespace VTX::UI::QT::Action::Viewpoint
