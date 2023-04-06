#ifndef __VTX_UI_QT_ACTION_VIEWPOINT__
#define __VTX_UI_QT_ACTION_VIEWPOINT__

#include "qt/controller/base_camera_controller.hpp"
#include <old/action/base_action.hpp>
#include <old/model/path.hpp>
#include <old/model/viewpoint.hpp>
#include <old/object3d/camera.hpp>
#include <vector>

namespace VTX::UI::QT::Action::Viewpoint
{
	class Create : public VTX::Action::BaseAction
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
		ID::VTX_ID	  _controller = VTX::Setting::CONTROLLER_MODE_DEFAULT;
	};

	class Delete : public VTX::Action::BaseAction
	{
	  public:
		explicit Delete( Model::Viewpoint & p_viewpoint ) : _viewpoints( { &p_viewpoint } )
		{
			_tag = VTX::Action::ACTION_TAG( _tag | VTX::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Delete( const std::vector<Model::Viewpoint *> & p_viewpoints ) : _viewpoints( p_viewpoints )
		{
			_tag = VTX::Action::ACTION_TAG( _tag | VTX::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::vector<Model::Viewpoint *> _viewpoints;
	};

	class GoTo : public VTX::Action::BaseAction
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
