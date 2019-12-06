#ifndef __VTX__TRANSFORM__
#define __VTX__TRANSFORM__

#ifdef _MSC_VER
#pragma once
#endif

#include "../defines.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace VTX
{
	namespace Math
	{
		class Transform
		{
		  public:
			Transform()	 = default;
			~Transform() = default;

			const Mat4f & get() const { return _transform; }
			void		  reset();

			void translate( const Vec3f & );
			void setTranslation( const Mat4f & );
			void rotate( const float p_angle, const Vec3f & p_axis );
			void setRotation( const Mat4f & );
			void scale( const Vec3f & );
			void setScale( const Mat4f & );

		  private:
			void update() { _transform = _translation * _rotation * _scale; }

			Mat4f _transform   = MAT4F_ID;
			Mat4f _translation = MAT4F_ID;
			Mat4f _rotation	   = MAT4F_ID;
			Mat4f _scale	   = MAT4F_ID;
		};
	} // namespace Math
} // namespace VTX

#endif
