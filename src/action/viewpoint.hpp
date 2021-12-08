#ifndef __VTX_ACTION_VIEWPOINT__
#define __VTX_ACTION_VIEWPOINT__

#include "base_action.hpp"
#include "controller/freefly.hpp"
#include "controller/trackball.hpp"
#include "model/path.hpp"
#include "model/selection.hpp"
#include "model/viewpoint.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "setting.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "vtx_app.hpp"
#include <set>
#include <vector>

namespace VTX::Action::Viewpoint
{
	class Create : public BaseAction
	{
	  public:
		explicit Create() :
			Create( *VTXApp::get().getScene().getPaths()[ 0 ],
					VTXApp::get().getScene().getCamera(),
					VTXApp::get()
						.getStateMachine()
						.getState<State::Visualization>( ID::State::VISUALIZATION )
						->getCurrentCameraController() )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit Create( Model::Path &							  p_path,
						 const Object3D::Camera &				  p_camera,
						 Controller::BaseCameraController * const p_controller ) :
			_path( p_path ),
			_rotation( p_camera.getRotation() ),
			_position( p_camera.getPosition() ), _controller (p_controller->getID())
		{
			const State::Visualization * const visualizationState
				= VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

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

		virtual void execute() override
		{
			Model::Viewpoint * const viewpoint
				= MVC::MvcManager::get().instantiateModel<Model::Viewpoint, Model::Path * const>( &_path );
			viewpoint->setController( _controller );
			viewpoint->setRotation( _rotation );
			viewpoint->setPosition( _position );

			viewpoint->setTarget( _target );
			viewpoint->setDistance( Util::Math::distance( _position, _target ) );

			_path.addViewpoint( viewpoint );
			_path.refreshAllDurations();
		}

	  private:
		Model::Path & _path;
		const Vec3f	  _position;
		const Quatf	  _rotation;
		Vec3f		  _target	  = VEC3F_ZERO;
		ID::VTX_ID	  _controller = VTX::Setting::CONTROLLER_MODE_DEFAULT;
	};

	class Delete : public BaseAction
	{
	  public:
		explicit Delete( Model::Viewpoint & p_viewpoint ) : _viewpoints( { &p_viewpoint } )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit Delete( const std::vector<Model::Viewpoint *> & p_viewpoints ) : _viewpoints( p_viewpoints )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			VTX::Selection::SelectionManager::get().getSelectionModel().unselectModels( _viewpoints );

			std::set<Model::Path *> paths = std::set<Model::Path *>();
			for ( Model::Viewpoint * const viewpoint : _viewpoints )
			{
				Model::Path * const path = viewpoint->getPathPtr();
				path->removeViewpoint( viewpoint );
				paths.emplace( path );

				MVC::MvcManager::get().deleteModel( viewpoint );
			}

			for ( Model::Path * const path : paths )
				path->refreshAllDurations();
		}

	  private:
		std::vector<Model::Viewpoint *> _viewpoints;
	};

	class GoTo : public BaseAction
	{
	  public:
		explicit GoTo( const Model::Viewpoint & p_viewpoint ) :
			GoTo( p_viewpoint, VTXApp::get().getScene().getCamera() )
		{
		}
		explicit GoTo( const Model::Viewpoint & p_viewpoint, const Object3D::Camera & p_camera ) :
			_viewpoint( p_viewpoint ), _camera( p_camera )
		{
		}

		virtual void execute() override
		{
			State::Visualization * const state
				= VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

			if ( state->getCurrentCameraControllerID() == ID::Controller::TRACKBALL )
			{
				Controller::Trackball * const trackball
					= dynamic_cast<Controller::Trackball * const>( state->getCurrentCameraController() );
				trackball->setTarget( _viewpoint.getTarget() );
			}

			VTXApp::get()
				.getStateMachine()
				.getState<State::Visualization>( ID::State::VISUALIZATION )
				->orientCameraController( _viewpoint.getPosition(), _viewpoint.getRotation() );
		}

	  private:
		const Model::Viewpoint & _viewpoint;
		const Object3D::Camera & _camera;
	};

	/*
	class AddAction : public BaseAction
	{
	  public:
		explicit AddAction( Model::Viewpoint & p_viewpoint, const std::string & p_action ) :
			_viewpoint( p_viewpoint ), _action( p_action )
		{
		}

		virtual void execute() override { _viewpoint.addAction( _action ); }

	  private:
		Model::Viewpoint & _viewpoint;
		const std::string  _action;
	};

	class DeleteAction : public BaseAction
	{
	  public:
		explicit DeleteAction( Model::Viewpoint &								p_viewpoint,
							   const std::vector<std::string>::const_iterator & p_action ) :
			_viewpoint( p_viewpoint ),
			_action( p_action )
		{
		}

		virtual void execute() override { _viewpoint.removeAction( _action ); }

	  private:
		Model::Viewpoint &							   _viewpoint;
		const std::vector<std::string>::const_iterator _action;
	};
	*/

	class ChangeDuration : public BaseAction
	{
	  public:
		explicit ChangeDuration( Model::Viewpoint & p_viewpoint, const float p_duration ) :
			_viewpoint( p_viewpoint ), _duration( p_duration )
		{
		}

		virtual void execute() override
		{
			_viewpoint.setDuration( Util::Math::max<float>( _duration, 0.f ) );
			_viewpoint.getPathPtr()->refreshAllDurations();
		}

	  private:
		Model::Viewpoint & _viewpoint;
		const float		   _duration;
	};

	class Relocate : public BaseAction
	{
	  public:
		explicit Relocate( Model::Viewpoint & p_viewpoint ) :
			Relocate( p_viewpoint, VTXApp::get().getScene().getCamera() )
		{
		}
		explicit Relocate( std::vector<Model::Viewpoint *> & p_viewpoints ) :
			Relocate( p_viewpoints, VTXApp::get().getScene().getCamera() )
		{
		}
		explicit Relocate( Model::Viewpoint & p_viewpoint, const Object3D::Camera & p_camera ) :
			_viewpoints { &p_viewpoint }, _position( p_camera.getPosition() ), _rotation( p_camera.getRotation() )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit Relocate( std::vector<Model::Viewpoint *> & p_viewpoints, const Object3D::Camera & p_camera ) :
			_viewpoints( p_viewpoints ), _position( p_camera.getPosition() ), _rotation( p_camera.getRotation() )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit Relocate( Model::Viewpoint & p_viewpoint, const Math::Transform & p_transform ) :
			_viewpoints { &p_viewpoint }, _position( p_transform.getTranslationVector() ),
			_rotation( p_transform.getRotation() )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit Relocate( std::vector<Model::Viewpoint *> & p_viewpoints, const Math::Transform & p_transform ) :
			_viewpoints( p_viewpoints ), _position( p_transform.getTranslationVector() ),
			_rotation( p_transform.getRotation() )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			std::set<Model::Path *> paths = std::set<Model::Path *>();

			for ( Model::Viewpoint * const viewpoint : _viewpoints )
			{
				viewpoint->setPosition( _position );
				viewpoint->setRotation( _rotation );
				paths.emplace( viewpoint->getPathPtr() );
			}

			for ( Model::Path * const path : paths )
				path->refreshAllDurations();
		}

	  private:
		std::vector<Model::Viewpoint *> _viewpoints;
		const Vec3f						_position;
		const Quatf						_rotation;
	};

	class Translate : public BaseAction
	{
	  public:
		explicit Translate( Model::Viewpoint & p_viewpoint, const Vec3f & p_translation ) :
			_viewpoints { &p_viewpoint }, _translation( p_translation )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit Translate( std::vector<Model::Viewpoint *> & p_viewpoints, const Vec3f & p_translation ) :
			_viewpoints( p_viewpoints ), _translation( p_translation )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			std::set<Model::Path *> paths = std::set<Model::Path *>();

			for ( Model::Viewpoint * const viewpoint : _viewpoints )
			{
				viewpoint->setPosition( viewpoint->getPosition() + _translation );
				paths.emplace( viewpoint->getPathPtr() );
			}

			for ( Model::Path * const path : paths )
				path->refreshAllDurations();
		}

	  private:
		std::vector<Model::Viewpoint *> _viewpoints;
		const Vec3f						_translation;
	};

	class Rotate : public BaseAction
	{
	  private:
		enum class RotationType
		{
			Axis_Angle,
			Euler
		};

	  public:
		explicit Rotate( Model::Viewpoint & p_viewpoint, const float p_angle, const Vec3f & p_axis ) :
			_viewpoints { &p_viewpoint }, _angle( p_angle ), _axis( p_axis ), _rotationType( RotationType::Axis_Angle )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit Rotate( const std::unordered_set<Model::Viewpoint *> & p_viewpoints,
						 const float									p_angle,
						 const Vec3f &									p_axis ) :
			_viewpoints( p_viewpoints ),
			_angle( p_angle ), _axis( p_axis ), _rotationType( RotationType::Axis_Angle )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		explicit Rotate( Model::Viewpoint & p_viewpoint, const Vec3f & p_euler ) :
			_viewpoints { &p_viewpoint }, _axis( p_euler ), _angle( 0 ), _rotationType( RotationType::Euler )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit Rotate( const std::unordered_set<Model::Viewpoint *> & p_viewpoints, const Vec3f & p_euler ) :
			_viewpoints( p_viewpoints ), _axis( p_euler ), _angle( 0 ), _rotationType( RotationType::Euler )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Model::Viewpoint * const viewpoint : _viewpoints )
			{
				switch ( _rotationType )
				{
				case RotationType::Axis_Angle:
				{
					Quatf newRotation = Util::Math::rotate( viewpoint->getRotation(), _angle, _axis );
					viewpoint->setRotation( newRotation );
					break;
				}
				case RotationType::Euler:
				{

					Quatf deltaRotation = Util::Math::eulerToQuaternion( _axis );
					viewpoint->setRotation( viewpoint->getRotation() + deltaRotation );
					break;
				}
				default: break;
				}
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Model::Viewpoint *> _viewpoints;
		RotationType						   _rotationType;
		const float							   _angle;
		const Vec3f							   _axis;
	};

	class Rename : public BaseAction
	{
	  public:
		explicit Rename( Model::Viewpoint & p_viewpoint, const std::string & p_name ) :
			_name( p_name ), _viewpoint( p_viewpoint )
		{
		}

		virtual void execute() override { _viewpoint.setName( _name ); }

	  private:
		Model::Viewpoint &	_viewpoint;
		const std::string & _name;
	};

} // namespace VTX::Action::Viewpoint
#endif
