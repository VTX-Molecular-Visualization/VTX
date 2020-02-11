#ifndef __VTX_ACTION_VIEWPOINT_GOTO__
#define __VTX_ACTION_VIEWPOINT_GOTO__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/path.hpp"
#include "model/viewpoint.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ViewpointGoTo : public BaseAction
		{
		  public:
			explicit ViewpointGoTo( Model::Viewpoint & p_viewpoint, Object3D::Camera & p_camera ) :
				_viewpoint( p_viewpoint ), _camera( p_camera )
			{
			}

			virtual void execute() override { _camera.set( _viewpoint.getPosition(), _viewpoint.getRotation() ); }

		  private:
			Object3D::Camera & _camera;
			Model::Viewpoint & _viewpoint;
		};
	} // namespace Action
} // namespace VTX
#endif
