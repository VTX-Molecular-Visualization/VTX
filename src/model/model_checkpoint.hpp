#ifndef __VTX_MODEL_CHECKPOINT__
#define __VTX_MODEL_CHECKPOINT__

#ifdef _MSC_VER
#pragma once
#endif

#include "../define.hpp"
#include "base_model.hpp"

namespace VTX
{
	namespace Model
	{
		class ModelCheckpoint : public BaseModel
		{
		  public:
			static uint COUNTER;

			explicit ModelCheckpoint( const Vec3f p_position, const float p_theta, const float p_phi ) :
				_position( p_position ), _theta( p_theta ), _phi( p_phi )
			{
				_id = COUNTER++;
			}

			inline float		 getDuration() const { return _duration; }
			inline float *		 getDurationPtr() { return &_duration; }
			inline const Vec3f & getPosition() const { return _position; }
			inline const float	 getTheta() const { return _theta; }
			inline const float	 getPhi() const { return _phi; }

		  private:
			float _duration = 1.f;

			const Vec3f _position = VEC3F_ZERO;
			const float _theta	  = 0.f;
			const float _phi	  = 4.22369f;

		}; // namespace Camera
	}	   // namespace Model
} // namespace VTX
#endif
