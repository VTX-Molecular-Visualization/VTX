#include "ui/qt/action/viewpoint.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/controller/freefly.hpp"
#include "ui/qt/controller/trackball.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/visualization.hpp"
#include <app/model/selection.hpp>
#include <app/mvc.hpp>
#include <app/old_app/object3d/scene.hpp>
#include <app/old_app/selection/selection_manager.hpp>
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

	Create::Create( Model::Path &							 p_path,
					const Object3D::Camera &				 p_camera,
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

	void Create::execute()
	{
		const std::string		 viewpointName = _path.generateNewViewpointName();
		Model::Viewpoint * const viewpoint
			= VTX::MVC_MANAGER().instantiateModel<Model::Viewpoint, Model::Path * const>( &_path );
		viewpoint->setController( _controller );
		viewpoint->setRotation( _rotation );
		viewpoint->setPosition( _position );

		viewpoint->setName( viewpointName );

		viewpoint->setTarget( _target );
		viewpoint->setDistance( Util::Math::distance( _position, _target ) );

		_path.addViewpoint( viewpoint );
		_path.refreshAllDurations();
	}

	void Delete::execute()
	{
		VTX::Selection::SelectionManager::get().getSelectionModel().unselectModels( _viewpoints );

		std::set<Model::Path *> paths = std::set<Model::Path *>();
		for ( Model::Viewpoint * const viewpoint : _viewpoints )
		{
			Model::Path * const path = viewpoint->getPathPtr();
			path->removeViewpoint( viewpoint );
			paths.emplace( path );

			VTX::MVC_MANAGER().deleteModel( viewpoint );
		}

		for ( Model::Path * const path : paths )
			path->refreshAllDurations();
	}

	GoTo::GoTo( const Model::Viewpoint & p_viewpoint ) : GoTo( p_viewpoint, VTXApp::get().getScene().getCamera() ) {}
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
