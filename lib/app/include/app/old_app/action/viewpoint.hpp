#ifndef __VTX_ACTION_VIEWPOINT__
#define __VTX_ACTION_VIEWPOINT__

#include "base_action.hpp"
#include "app/old_app/model/path.hpp"
#include "app/old_app/model/selection.hpp"
#include "app/old_app/model/viewpoint.hpp"
#include "app/old_app/mvc/mvc_manager.hpp"
#include "app/old_app/object3d/camera.hpp"
#include "app/old_app/object3d/scene.hpp"
#include "app/old_app/selection/selection_manager.hpp"
#include "app/old_app/setting.hpp"
#include "app/old_app/vtx_app.hpp"
#include <set>
#include <vector>

namespace VTX::Action::Viewpoint
{
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
