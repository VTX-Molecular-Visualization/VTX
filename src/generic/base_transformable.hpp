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
			inline virtual const Math::Transform & getTransform() const { return _transform; };

			void rotate( const float, const Vec3f & );
			void setScale( const float & );
			void setScale( const Mat4f & );
			void setTranslation( const Vec3f & );
			void setTranslation( const Mat4f & );
			void applyTransform( const Math::Transform & );

		  protected:
			Math::Transform _transform;
			virtual void	_transformModifiedEvent() {};
		};
	} // namespace Generic
} // namespace VTX
#endif
