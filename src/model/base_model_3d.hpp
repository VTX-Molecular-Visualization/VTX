#ifndef __VTX_BASE_MODEL_3D__
#define __VTX_BASE_MODEL_3D__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/base_renderable.hpp"
#include "generic/base_transformable.hpp"
#include "generic/base_visible.hpp"
#include "model/base_model.hpp"

namespace VTX
{
	namespace Model
	{
		class BaseModel3D :
			public BaseModel,
			public Generic::BaseTransformable,
			public Generic::BaseRenderable,
			public Generic::BaseVisible
		{
		  public:
			virtual ~BaseModel3D() = default;
		};
	} // namespace Model
} // namespace VTX
#endif
