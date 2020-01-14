#ifndef __VTX_BASE_TRANSFORMABLE__
#define __VTX_BASE_TRANSFORMABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../math/transform.hpp"

namespace VTX
{
	namespace Generic
	{
		class BaseTransformable
		{
		  public:
			inline Math::Transform & getTransform() { return _transform; };

		  protected:
			Math::Transform _transform;
		};
	} // namespace Generic
} // namespace VTX
#endif
