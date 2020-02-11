#ifndef __VTX_ACTION_VIEWPOINT_REPLACE__
#define __VTX_ACTION_VIEWPOINT_REPLACE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../object3d/camera.hpp"
#include "base_action.hpp"
#include "model/viewpoint.hpp"

namespace VTX
{
	namespace Action
	{
		class ViewpointReplace : public BaseAction
		{
		  public:
			explicit ViewpointReplace( Model::Viewpoint & p_viewpoint, const Object3D::Camera & p_camera ) :
				_viewpoint( p_viewpoint ), _position( p_camera.getPosition() ), _rotation( p_camera.getRotation() )
			{
			}

			virtual void execute() override
			{
				_viewpoint.setPosition( _position );
				_viewpoint.setRotation( _rotation );
			}

		  private:
			Model::Viewpoint & _viewpoint;
			const Vec3f		   _position;
			const Quatf		   _rotation;
		};
	} // namespace Action
} // namespace VTX
#endif
