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
			explicit ActionCheckpointCreate( const Object3D::Camera p_camera ) : _camera( p_camera ) {}

			virtual void execute() override
			{
				// VTXApp::get().getScene().getPaths()[ 0 ]->addCheckpoint(
				//	new Model::ModelCheckpoint( _camera.getPosition(), _camera.getTheta(), _camera.getPhi() ) );
			}

		  private:
			Object3D::Camera _camera;
		};
	} // namespace Action
} // namespace VTX
#endif
