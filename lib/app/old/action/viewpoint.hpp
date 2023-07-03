#ifndef __VTX_APP_OLD_ACTION_VIEWPOINT__
#define __VTX_APP_OLD_ACTION_VIEWPOINT__

#include "app/old/action.hpp"
#include "app/old/component/render/camera.hpp"
#include "app/old/component/object3d/viewpoint.hpp"
#include "app/old/core/action/base_action.hpp"
#include "app/old/internal/math/transform.hpp"
#include "app/old/vtx_app.hpp"
#include "app/old/application/scene.hpp"
#include <string>
#include <unordered_set>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Old::Action::Viewpoint
{
	// class AddAction : public App::Old::Core::Action::BaseAction
	//{
	//   public:
	//	explicit AddAction( App::Old::Component::Object3D::Viewpoint & p_viewpoint, const std::string & p_action ) :
	//		_viewpoint( p_viewpoint ), _action( p_action )
	//	{
	//	}

	//	virtual void execute() override;

	//  private:
	//	App::Old::Component::Object3D::Viewpoint & _viewpoint;
	//	const std::string  _action;
	//};

	// class DeleteAction : public App::Old::Core::Action::BaseAction
	//{
	//   public:
	//	explicit DeleteAction( App::Old::Component::Object3D::Viewpoint &								p_viewpoint,
	//						   const std::vector<std::string>::const_iterator & p_action ) :
	//		_viewpoint( p_viewpoint ),
	//		_action( p_action )
	//	{
	//	}

	//	virtual void execute() override;

	//  private:
	//	App::Old::Component::Object3D::Viewpoint &							   _viewpoint;
	//	const std::vector<std::string>::const_iterator _action;
	//};

	class ChangeDuration : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeDuration( App::Old::Component::Object3D::Viewpoint & p_viewpoint, const float p_duration ) :
			_viewpoint( p_viewpoint ), _duration( p_duration )
		{
		}

		virtual void execute() override;

	  private:
		App::Old::Component::Object3D::Viewpoint & _viewpoint;
		const float							  _duration;
	};

	class Relocate : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Relocate( App::Old::Component::Object3D::Viewpoint & p_viewpoint ) :
			Relocate( p_viewpoint, App::Old::VTXApp::get().getScene().getCamera() )
		{
		}
		explicit Relocate( std::vector<App::Old::Component::Object3D::Viewpoint *> & p_viewpoints ) :
			Relocate( p_viewpoints, App::Old::VTXApp::get().getScene().getCamera() )
		{
		}
		explicit Relocate( App::Old::Component::Object3D::Viewpoint &	p_viewpoint,
						   const App::Old::Component::Render::Camera & p_camera ) :
			_viewpoints { &p_viewpoint },
			_position( p_camera.getPosition() ), _rotation( p_camera.getRotation() )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Relocate( std::vector<App::Old::Component::Object3D::Viewpoint *> & p_viewpoints,
						   const App::Old::Component::Render::Camera &				p_camera ) :
			_viewpoints( p_viewpoints ),
			_position( p_camera.getPosition() ), _rotation( p_camera.getRotation() )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Relocate( App::Old::Component::Object3D::Viewpoint & p_viewpoint, const App::Old::Internal::Math::Transform & p_transform ) :
			_viewpoints { &p_viewpoint }, _position( p_transform.getTranslationVector() ),
			_rotation( p_transform.getRotation() )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Relocate( std::vector<App::Old::Component::Object3D::Viewpoint *> & p_viewpoints,
						   const App::Old::Internal::Math::Transform &								p_transform ) :
			_viewpoints( p_viewpoints ),
			_position( p_transform.getTranslationVector() ), _rotation( p_transform.getRotation() )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::vector<App::Old::Component::Object3D::Viewpoint *> _viewpoints;
		const Vec3f										   _position;
		const Quatf										   _rotation;
	};

	class Translate : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Translate( App::Old::Component::Object3D::Viewpoint & p_viewpoint, const Vec3f & p_translation ) :
			_viewpoints { &p_viewpoint }, _translation( p_translation )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Translate( std::vector<App::Old::Component::Object3D::Viewpoint *> & p_viewpoints,
							const Vec3f &										 p_translation ) :
			_viewpoints( p_viewpoints ),
			_translation( p_translation )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::vector<App::Old::Component::Object3D::Viewpoint *> _viewpoints;
		const Vec3f										   _translation;
	};

	class Rotate : public App::Old::Core::Action::BaseAction
	{
	  private:
		enum class RotationType
		{
			Axis_Angle,
			Euler
		};

	  public:
		explicit Rotate( App::Old::Component::Object3D::Viewpoint & p_viewpoint,
						 const float						   p_angle,
						 const Vec3f &						   p_axis ) :
			_viewpoints { &p_viewpoint },
			_angle( p_angle ), _axis( p_axis ), _rotationType( RotationType::Axis_Angle )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Rotate( const std::unordered_set<App::Old::Component::Object3D::Viewpoint *> & p_viewpoints,
						 const float													   p_angle,
						 const Vec3f &													   p_axis ) :
			_viewpoints( p_viewpoints ),
			_angle( p_angle ), _axis( p_axis ), _rotationType( RotationType::Axis_Angle )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		explicit Rotate( App::Old::Component::Object3D::Viewpoint & p_viewpoint, const Vec3f & p_euler ) :
			_viewpoints { &p_viewpoint }, _axis( p_euler ), _angle( 0 ), _rotationType( RotationType::Euler )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Rotate( const std::unordered_set<App::Old::Component::Object3D::Viewpoint *> & p_viewpoints,
						 const Vec3f &													   p_euler ) :
			_viewpoints( p_viewpoints ),
			_axis( p_euler ), _angle( 0 ), _rotationType( RotationType::Euler )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<App::Old::Component::Object3D::Viewpoint *> _viewpoints;
		RotationType											  _rotationType;
		const float												  _angle;
		const Vec3f												  _axis;
	};

	class Rename : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Rename( App::Old::Component::Object3D::Viewpoint & p_viewpoint, const std::string & p_name ) :
			_name( p_name ), _viewpoint( p_viewpoint )
		{
		}

		virtual void execute() override;

	  private:
		App::Old::Component::Object3D::Viewpoint & _viewpoint;
		const std::string &					  _name;
	};

} // namespace VTX::App::Old::Action::Viewpoint
#endif
