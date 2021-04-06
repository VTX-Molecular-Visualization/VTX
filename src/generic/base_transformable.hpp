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
			enum TransformComposantMask
			{
				TRANSLATE_X = 1 << 0,
				TRANSLATE_Y = 1 << 1,
				TRANSLATE_Z = 1 << 2,

				TRANSLATE_VECTOR = TRANSLATE_X | TRANSLATE_Y | TRANSLATE_Z,

				EULER_X = 1 << 3,
				EULER_Y = 1 << 4,
				EULER_Z = 1 << 5,

				EULER_VECTOR = EULER_X | EULER_Y | EULER_Z,

				SCALE_X = 1 << 6,
				SCALE_Y = 1 << 7,
				SCALE_Z = 1 << 8,

				SCALE_VECTOR = SCALE_X | SCALE_Y | SCALE_Z,

				TRANSFORM = TRANSLATE_VECTOR | EULER_VECTOR | SCALE_VECTOR,
			};

			inline virtual const Math::Transform & getTransform() const { return _transform; };

			void rotate( const float, const Vec3f & );
			void setScale( const float & );
			void setScale( const Mat4f & );
			void setTranslation( const Vec3f & );
			void setTranslation( const Mat4f & );
			void applyTransform( const Math::Transform & );
			void applyTransform( const Math::Transform &, const TransformComposantMask );

		  protected:
			Math::Transform _transform;
			virtual void	_transformModifiedEvent() {};
		};
	} // namespace Generic
} // namespace VTX
#endif
