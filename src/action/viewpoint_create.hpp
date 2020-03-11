#ifndef __VTX_ACTION_VIEWPOINT_CREATE__
#define __VTX_ACTION_VIEWPOINT_CREATE__

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
		class ViewpointCreate : public BaseAction
		{
		  public:
			explicit ViewpointCreate( Model::Path & p_path, const Object3D::Camera & p_camera ) :
				_path( p_path ), _position( p_camera.getPosition() ), _rotation( p_camera.getRotation() )
			{
			}

			virtual void execute() override
			{
				Model::Viewpoint * const viewpoint = new Model::Viewpoint( &_path, _position, _rotation );
				_path.addViewpoint( viewpoint );
				_path.refreshAllDurations();
			}

		  private:
			Model::Path & _path;
			const Vec3f	  _position;
			const Quatf	  _rotation;
		};
	} // namespace Action
} // namespace VTX
#endif
