#ifndef __VTX_ACTION_CHECKPOINT_REPLACE__
#define __VTX_ACTION_CHECKPOINT_REPLACE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../object3d/camera.hpp"
#include "base_action.hpp"
#include "model/checkpoint.hpp"

namespace VTX
{
	namespace Action
	{
		class CheckpointReplace : public BaseAction
		{
		  public:
			explicit CheckpointReplace( Model::Checkpoint & p_checkpoint, const Object3D::Camera & p_camera ) :
				_checkpoint( p_checkpoint ), _position( p_camera.getPosition() ), _rotation( p_camera.getRotation() )
			{
			}

			virtual void execute() override
			{
				_checkpoint.setPosition( _position );
				_checkpoint.setRotation( _rotation );
			}

		  private:
			Model::Checkpoint & _checkpoint;
			const Vec3f			_position;
			const Quatf			_rotation;
		};
	} // namespace Action
} // namespace VTX
#endif
