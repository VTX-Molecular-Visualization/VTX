#ifndef __VTX_ACTION_VIEWPOINT__
#define __VTX_ACTION_VIEWPOINT__

#include "base_action.hpp"
#include "controller/trackball.hpp"
#include "model/path.hpp"
#include "model/selection.hpp"
#include "model/viewpoint.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "setting.hpp"
#include "vtx_app.hpp"
#include <set>
#include <vector>

namespace VTX
{
	namespace Action
	{
		namespace Viewpoint
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
				}
				explicit Create( Model::Path &							  p_path,
								 const Object3D::Camera &				  p_camera,
								 Controller::BaseCameraController * const p_controller ) :
					_path( p_path ),
					_rotation( p_camera.getRotation() ), _position( p_camera.getPosition() )
				{
					const Controller::Trackball * const trackball
						= dynamic_cast<Controller::Trackball * const>( p_controller );
					if ( trackball != nullptr )
					{
						_target		= trackball->getTarget();
						_controller = ID::Controller::TRACKBALL;
					}
					else
					{
						_controller = ID::Controller::FREEFLY;
					}
				}

				virtual void execute() override
				{
					Model::Viewpoint * const viewpoint
						= MVC::MvcManager::get().instantiateModel<Model::Viewpoint, Model::Path * const>( &_path );
					viewpoint->setController( _controller );
					viewpoint->setRotation( _rotation );
					viewpoint->setPosition( _position );

					if ( _controller == ID::Controller::TRACKBALL )
					{
						viewpoint->setTarget( _target );
						viewpoint->setDistance( Util::Math::distance( _position, _target ) );
					}

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
				explicit Delete( Model::Viewpoint & p_viewpoint ) : _viewpoints( { &p_viewpoint } ) {}
				explicit Delete( const std::vector<Model::Viewpoint *> & p_viewpoints ) : _viewpoints( p_viewpoints ) {}

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
				explicit GoTo( Model::Viewpoint & p_viewpoint ) :
					GoTo( p_viewpoint, VTXApp::get().getScene().getCamera() )
				{
				}
				explicit GoTo( Model::Viewpoint & p_viewpoint, Object3D::Camera & p_camera ) :
					_viewpoints { &p_viewpoint }, _camera( p_camera )
				{
				}
				explicit GoTo( std::vector<Model::Viewpoint *> & p_viewpoints ) :
					GoTo( p_viewpoints, VTXApp::get().getScene().getCamera() )
				{
				}
				explicit GoTo( std::vector<Model::Viewpoint *> & p_viewpoints, Object3D::Camera & p_camera ) :
					_viewpoints( p_viewpoints ), _camera( p_camera )
				{
				}

				virtual void execute() override
				{
					for ( Model::Viewpoint * const viewpoint : _viewpoints )
						_camera.set( viewpoint->getPosition(), viewpoint->getRotation() );
				}

			  private:
				std::vector<Model::Viewpoint *> _viewpoints;
				Object3D::Camera &				_camera;
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
					_viewpoints { &p_viewpoint }, _position( p_camera.getPosition() ),
					_rotation( p_camera.getRotation() )
				{
				}
				explicit Relocate( std::vector<Model::Viewpoint *> & p_viewpoints, const Object3D::Camera & p_camera ) :
					_viewpoints( p_viewpoints ), _position( p_camera.getPosition() ),
					_rotation( p_camera.getRotation() )
				{
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

		} // namespace Viewpoint
	}	  // namespace Action
} // namespace VTX
#endif
