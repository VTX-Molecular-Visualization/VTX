#ifndef __VTX_UI_QT_ACTION_VIEWPOINT__
#define __VTX_UI_QT_ACTION_VIEWPOINT__

#include "ui/qt/controller/base_camera_controller.hpp"
#include <app/core/action/base_action.hpp>
#include <app/model/path.hpp>
#include <app/model/viewpoint.hpp>
#include <app/old_app/object3d/camera.hpp>
#include <vector>

namespace VTX::UI::QT::Action::Viewpoint
{
	class Create : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit Create();
		explicit Create( Model::Path &							  p_path,
						 const Object3D::Camera &				  p_camera,
						 Controller::BaseCameraController * const p_controller );

		virtual void execute() override;

	  private:
		Model::Path & _path;
		const Vec3f	  _position;
		const Quatf	  _rotation;
		Vec3f		  _target	  = VEC3F_ZERO;
		ID::VTX_ID	  _controller = VTX::App::Application::Setting::CONTROLLER_MODE_DEFAULT;
	};

	class Delete : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit Delete( Model::Viewpoint & p_viewpoint ) : _viewpoints( { &p_viewpoint } )
		{
			_tag = VTX::App::Core::Action::ACTION_TAG( _tag | VTX::App::Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Delete( const std::vector<Model::Viewpoint *> & p_viewpoints ) : _viewpoints( p_viewpoints )
		{
			_tag = VTX::App::Core::Action::ACTION_TAG( _tag | VTX::App::Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::vector<Model::Viewpoint *> _viewpoints;
	};

	class GoTo : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit GoTo( const Model::Viewpoint & p_viewpoint );
		explicit GoTo( const Model::Viewpoint & p_viewpoint, const Object3D::Camera & p_camera ) :
			_viewpoint( p_viewpoint ), _camera( p_camera )
		{
		}

		virtual void execute() override;

	  private:
		const Model::Viewpoint & _viewpoint;
		const Object3D::Camera & _camera;
	};

} // namespace VTX::UI::QT::Action::Viewpoint
#endif
