#ifndef __VTX_MODEL_CHECKPOINT__
#define __VTX_MODEL_CHECKPOINT__

#ifdef _MSC_VER
#pragma once
#endif

#include "../defines.hpp"
#include "base_model.hpp"

namespace VTX
{
	namespace Model
	{
		class ModelCheckpoint : public BaseModel
		{
		  public:
			float _time = 0.f;

			Vec3f _position = VEC3F_ZERO;
			float _theta	= 0.f;
			float _phi		= 4.22369f;

		  protected:
			virtual void _addViews() override final;

		}; // namespace Camera
	}	   // namespace Model
} // namespace VTX
#endif
