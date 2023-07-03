#ifndef __VTX_UI_QT_ACTION_VIEWPOINT__
#define __VTX_UI_QT_ACTION_VIEWPOINT__

#include "ui/qt/controller/base_camera_controller.hpp"
#include <app/old/component/object3d/viewpoint.hpp>
#include <app/old/component/render/camera.hpp>
#include <app/old/component/video/path.hpp>
#include <app/old/core/action/base_action.hpp>
#include <vector>

namespace VTX::UI::QT::Action::Viewpoint
{
	class Create : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Create();
		explicit Create( App::Old::Component::Video::Path &			  p_path,
						 const App::Old::Component::Render::Camera &	  p_camera,
						 Controller::BaseCameraController * const p_controller );

		virtual void execute() override;

	  private:
		std::string _generateViewpointName() const;

		App::Old::Component::Video::Path & _path;
		const Vec3f					  _position;
		const Quatf					  _rotation;
		Vec3f						  _target	  = VEC3F_ZERO;
	 App::Old::VTX_ID					  _controller = VTX::App::Old::Application::Setting::CONTROLLER_MODE_DEFAULT;
	};

	class Delete : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Delete( App::Old::Component::Object3D::Viewpoint & p_viewpoint ) : _viewpoints( { &p_viewpoint } )
		{
			_tag = VTX::App::Old::Core::Action::ACTION_TAG( _tag | VTX::App::Old::Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Delete( const std::vector<App::Old::Component::Object3D::Viewpoint *> & p_viewpoints ) :
			_viewpoints( p_viewpoints )
		{
			_tag = VTX::App::Old::Core::Action::ACTION_TAG( _tag | VTX::App::Old::Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::vector<App::Old::Component::Object3D::Viewpoint *> _viewpoints;
	};

	class GoTo : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		explicit GoTo( const App::Old::Component::Object3D::Viewpoint & p_viewpoint );
		explicit GoTo( const App::Old::Component::Object3D::Viewpoint & p_viewpoint,
					   const App::Old::Component::Render::Camera &	   p_camera ) :
			_viewpoint( p_viewpoint ),
			_camera( p_camera )
		{
		}

		virtual void execute() override;

	  private:
		const App::Old::Component::Object3D::Viewpoint & _viewpoint;
		const App::Old::Component::Render::Camera &		_camera;
	};

} // namespace VTX::UI::QT::Action::Viewpoint
#endif
