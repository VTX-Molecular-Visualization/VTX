#ifndef __VTX_APP_ACTION_VIEWPOINT__
#define __VTX_APP_ACTION_VIEWPOINT__

#include "app/action.hpp"
#include "app/core/action/base_action.hpp"
#include "app/model/viewpoint.hpp"
#include "app/old_app/math/transform.hpp"
#include "app/old_app/object3d/camera.hpp"
#include "app/old_app/object3d/scene.hpp"
#include "app/old_app/vtx_app.hpp"
#include <string>
#include <unordered_set>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Viewpoint
{
	// class AddAction : public App::Core::Action::BaseAction
	//{
	//   public:
	//	explicit AddAction( Model::Viewpoint & p_viewpoint, const std::string & p_action ) :
	//		_viewpoint( p_viewpoint ), _action( p_action )
	//	{
	//	}

	//	virtual void execute() override;

	//  private:
	//	Model::Viewpoint & _viewpoint;
	//	const std::string  _action;
	//};

	// class DeleteAction : public App::Core::Action::BaseAction
	//{
	//   public:
	//	explicit DeleteAction( Model::Viewpoint &								p_viewpoint,
	//						   const std::vector<std::string>::const_iterator & p_action ) :
	//		_viewpoint( p_viewpoint ),
	//		_action( p_action )
	//	{
	//	}

	//	virtual void execute() override;

	//  private:
	//	Model::Viewpoint &							   _viewpoint;
	//	const std::vector<std::string>::const_iterator _action;
	//};

	class ChangeDuration : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeDuration( Model::Viewpoint & p_viewpoint, const float p_duration ) :
			_viewpoint( p_viewpoint ), _duration( p_duration )
		{
		}

		virtual void execute() override;

	  private:
		Model::Viewpoint & _viewpoint;
		const float		   _duration;
	};

	class Relocate : public App::Core::Action::BaseAction
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
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Relocate( std::vector<Model::Viewpoint *> & p_viewpoints, const Object3D::Camera & p_camera ) :
			_viewpoints( p_viewpoints ), _position( p_camera.getPosition() ), _rotation( p_camera.getRotation() )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Relocate( Model::Viewpoint & p_viewpoint, const Math::Transform & p_transform ) :
			_viewpoints { &p_viewpoint }, _position( p_transform.getTranslationVector() ),
			_rotation( p_transform.getRotation() )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Relocate( std::vector<Model::Viewpoint *> & p_viewpoints, const Math::Transform & p_transform ) :
			_viewpoints( p_viewpoints ), _position( p_transform.getTranslationVector() ),
			_rotation( p_transform.getRotation() )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::vector<Model::Viewpoint *> _viewpoints;
		const Vec3f						_position;
		const Quatf						_rotation;
	};

	class Translate : public App::Core::Action::BaseAction
	{
	  public:
		explicit Translate( Model::Viewpoint & p_viewpoint, const Vec3f & p_translation ) :
			_viewpoints { &p_viewpoint }, _translation( p_translation )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Translate( std::vector<Model::Viewpoint *> & p_viewpoints, const Vec3f & p_translation ) :
			_viewpoints( p_viewpoints ), _translation( p_translation )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::vector<Model::Viewpoint *> _viewpoints;
		const Vec3f						_translation;
	};

	class Rotate : public App::Core::Action::BaseAction
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
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Rotate( const std::unordered_set<Model::Viewpoint *> & p_viewpoints,
						 const float									p_angle,
						 const Vec3f &									p_axis ) :
			_viewpoints( p_viewpoints ),
			_angle( p_angle ), _axis( p_axis ), _rotationType( RotationType::Axis_Angle )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		explicit Rotate( Model::Viewpoint & p_viewpoint, const Vec3f & p_euler ) :
			_viewpoints { &p_viewpoint }, _axis( p_euler ), _angle( 0 ), _rotationType( RotationType::Euler )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Rotate( const std::unordered_set<Model::Viewpoint *> & p_viewpoints, const Vec3f & p_euler ) :
			_viewpoints( p_viewpoints ), _axis( p_euler ), _angle( 0 ), _rotationType( RotationType::Euler )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Model::Viewpoint *> _viewpoints;
		RotationType						   _rotationType;
		const float							   _angle;
		const Vec3f							   _axis;
	};

	class Rename : public App::Core::Action::BaseAction
	{
	  public:
		explicit Rename( Model::Viewpoint & p_viewpoint, const std::string & p_name ) :
			_name( p_name ), _viewpoint( p_viewpoint )
		{
		}

		virtual void execute() override;

	  private:
		Model::Viewpoint &	_viewpoint;
		const std::string & _name;
	};

} // namespace VTX::App::Action::Viewpoint
#endif
