#ifndef __VTX_ACTION_VIEWPOINT__
#define __VTX_ACTION_VIEWPOINT__

#include "base_action.hpp"
#include "controller/trackball.hpp"
#include "model/path.hpp"
#include "model/viewpoint.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "setting.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		namespace Viewpoint
		{
			class Create : public BaseAction
			{
			  public:
				explicit Create( Model::Path &						p_path,
								 const Object3D::Camera &			p_camera,
								 Controller::BaseController * const p_controller ) :
					_path( p_path ),
					_rotation( p_camera.getRotation() ), _position( p_camera.getPosition() )
				{
					const Controller::Trackball * const trackball
						= dynamic_cast<Controller::Trackball *>( p_controller );
					if ( trackball != nullptr )
					{
						_target		= trackball->getTarget();
						_distance	= Util::Math::distance( _position, _target );
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
					viewpoint->setPosition( _position );
					viewpoint->setRotation( _rotation );
					viewpoint->setTarget( _target );
					viewpoint->setDistance( _distance );
					_path.addViewpoint( viewpoint );
					_path.refreshAllDurations();
				}

			  private:
				Model::Path & _path;
				const Vec3f	  _position;
				const Quatf	  _rotation;
				Vec3f		  _target	  = VEC3F_ZERO;
				float		  _distance	  = 0.f;
				ID::VTX_ID	  _controller = VTX::Setting::CONTROLLER_MODE_DEFAULT;
			};

			class Delete : public BaseAction
			{
			  public:
				explicit Delete( Model::Viewpoint & p_viewpoint ) : _viewpoint( p_viewpoint ) {}

				virtual void execute() override
				{
					Model::Path * const path = _viewpoint.getPathPtr();
					path->removeViewpoint( &_viewpoint );
					path->refreshAllDurations();
					delete &_viewpoint;
				}

			  private:
				Model::Viewpoint & _viewpoint;
			};

			class GoTo : public BaseAction
			{
			  public:
				explicit GoTo( Model::Viewpoint & p_viewpoint, Object3D::Camera & p_camera ) :
					_viewpoint( p_viewpoint ), _camera( p_camera )
				{
				}

				virtual void execute() override { _camera.set( _viewpoint.getPosition(), _viewpoint.getRotation() ); }

			  private:
				Object3D::Camera & _camera;
				Model::Viewpoint & _viewpoint;
			};

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

			class Replace : public BaseAction
			{
			  public:
				explicit Replace( Model::Viewpoint & p_viewpoint, const Object3D::Camera & p_camera ) :
					_viewpoint( p_viewpoint ), _position( p_camera.getPosition() ), _rotation( p_camera.getRotation() )
				{
				}

				virtual void execute() override
				{
					_viewpoint.setPosition( _position );
					_viewpoint.setRotation( _rotation );
					_viewpoint.getPathPtr()->refreshAllDurations();
				}

			  private:
				Model::Viewpoint & _viewpoint;
				const Vec3f		   _position;
				const Quatf		   _rotation;
			};
		} // namespace Viewpoint
	}	  // namespace Action
} // namespace VTX
#endif
