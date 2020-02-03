#ifndef __VTX_MODEL_CHECKPOINT__
#define __VTX_MODEL_CHECKPOINT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model.hpp"
#include "define.hpp"
#include <glm/gtc/quaternion.hpp>

namespace VTX
{
	namespace Model
	{
		class ModelCheckpoint : public BaseModel
		{
		  public:
			static uint COUNTER;

			struct CheckpointInterpolationData
			{
				explicit CheckpointInterpolationData( const Vec3f & p_positionLhs,
													  const Quatf & p_rotationLhs,
													  const Vec3f & p_positionRhs,
													  const Quatf & p_rotationRhs,
													  const float	p_value ) :
					positionLhs( p_positionLhs ),
					rotationLhs( p_rotationLhs ), positionRhs( p_positionRhs ), rotationRhs( p_rotationRhs ),
					value( p_value )
				{
				}
				const Vec3f & positionLhs;
				const Quatf & rotationLhs;
				const Vec3f & positionRhs;
				const Quatf & rotationRhs;
				const float	  value;
			};

			explicit ModelCheckpoint( const Vec3f & p_position, const Quatf & p_rotation ) :
				_position( p_position ), _rotation( p_rotation )
			{
				_id = COUNTER++;
			}

			inline float		 getDuration() const { return _duration; }
			inline float *		 getDurationPtr() { return &_duration; }
			inline const Vec3f & getPosition() const { return _position; }
			inline const Quatf & getRotation() const { return _rotation; }

		  private:
			float _duration = 1.f;

			Vec3f _position;
			Quatf _rotation;

		}; // namespace Camera
	}	   // namespace Model
} // namespace VTX
#endif
