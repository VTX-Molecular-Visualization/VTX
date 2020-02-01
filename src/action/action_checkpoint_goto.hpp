#ifndef __VTX_ACTION_CHECKPOINT_GOTO__
#define __VTX_ACTION_CHECKPOINT_GOTO__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/model_checkpoint.hpp"
#include "../model/model_path.hpp"
#include "../object3d/camera.hpp"
#include "../object3d/scene.hpp"
#include "../vtx_app.hpp"
#include "base_action.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionCheckpointGoTo : public BaseAction
		{
		  public:
			explicit ActionCheckpointGoTo( Object3D::Camera & p_camera, Model::ModelCheckpoint p_checkpoint ) :
				_camera( p_camera ), _checkpoint( p_checkpoint )
			{
			}

			virtual void execute() override
			{
				_camera.set( _checkpoint.getPosition(), _checkpoint.getTheta(), _checkpoint.getPhi() );
			}

		  private:
			Object3D::Camera &	   _camera;
			Model::ModelCheckpoint _checkpoint;
		};
	} // namespace Action
} // namespace VTX
#endif
