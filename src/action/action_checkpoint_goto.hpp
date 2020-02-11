#ifndef __VTX_ACTION_CHECKPOINT_GOTO__
#define __VTX_ACTION_CHECKPOINT_GOTO__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/checkpoint.hpp"
#include "model/path.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionCheckpointGoTo : public BaseAction
		{
		  public:
			explicit ActionCheckpointGoTo( Model::Checkpoint & p_checkpoint, Object3D::Camera & p_camera ) :
				_checkpoint( p_checkpoint ), _camera( p_camera )
			{
			}

			virtual void execute() override { _camera.set( _checkpoint.getPosition(), _checkpoint.getRotation() ); }

		  private:
			Object3D::Camera &		 _camera;
			Model::Checkpoint & _checkpoint;
		};
	} // namespace Action
} // namespace VTX
#endif
