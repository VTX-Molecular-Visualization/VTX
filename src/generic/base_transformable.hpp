#ifndef __VTX_BASE_TRANSFORMABLE__
#define __VTX_BASE_TRANSFORMABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "math/transform.hpp"

namespace VTX
{
	namespace Generic
	{
		class BaseTransformable
		{
		  public:
			virtual ~BaseTransformable() = default;

			inline const Math::Transform & getTransform() const { return _transform; };

			inline void rotate( const float p_angle, const Vec3f & p_axis ) { _transform.rotate( p_angle, p_axis ); }

			inline void setScale( const float & p_scale ) { _transform.setScale( p_scale ); }
			inline void setScale( const Mat4f & p_scale ) { _transform.setScale( p_scale ); }

			inline void setTranslation( const Vec3f & p_translation ) { _transform.setTranslation( p_translation ); }
			inline void setTranslation( const Mat4f & p_translation ) { _transform.setTranslation( p_translation ); }

		  protected:
			Math::Transform _transform;
		};
	} // namespace Generic
} // namespace VTX
#endif
