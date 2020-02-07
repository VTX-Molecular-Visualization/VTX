#ifndef __VTX_ACTION_CHECKPOINT_CREATE__
#define __VTX_ACTION_CHECKPOINT_CREATE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/model_checkpoint.hpp"
#include "model/model_path.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionCheckpointCreate : public BaseAction
		{
		  public:
			explicit ActionCheckpointCreate( Model::ModelPath & p_path, const Object3D::Camera & p_camera ) :
				_path( p_path ), _position( p_camera.getPosition() ), _rotation( p_camera.getRotation() )
			{
			}

			virtual void execute() override
			{
				_path.addCheckpoint( new Model::ModelCheckpoint( &_path, _position, _rotation ) );
			}

		  private:
			Model::ModelPath & _path;
			const Vec3f		   _position;
			const Quatf		   _rotation;
		};
	} // namespace Action
} // namespace VTX
#endif
