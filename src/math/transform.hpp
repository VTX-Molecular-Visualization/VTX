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

			inline const Mat4f get() const { return _transform; }

			inline const Mat4f & getTranslation() const { return _translation; };
			inline const Mat4f & getRotation() const { return _rotation; };
			inline const Mat4f & getScale() const { return _scale; };

			inline void Transform::reset()
			{
				_transform	 = MAT4F_ID;
				_translation = MAT4F_ID;
				_rotation	 = MAT4F_ID;
				_scale		 = MAT4F_ID;
			}

			inline void Transform::translate( const Vec3f & p_vec )
			{
				_translation = glm::translate( _translation, p_vec );
				update();
			}

			inline void Transform::setTranslation( const Mat4f & p_mat )
			{
				_translation = p_mat;
				update();
			}

			inline void Transform::rotate( const float p_angle, const Vec3f & p_axis )
			{
				_rotation = glm::rotate( _rotation, p_angle, p_axis );
				update();
			}

			inline void Transform::Transform::setRotation( const Mat4f & p_mat )
			{
				_rotation = p_mat;
				update();
			}

			inline void Transform::scale( const Vec3f & p_vec )
			{
				_scale = glm::scale( _scale, p_vec );
				update();
			}

			inline void Transform::scale( const float p_scale )
			{
				_scale = glm::scale( _scale, Vec3f( p_scale, p_scale, p_scale ) );
				update();
			}

			inline void Transform::setScale( const Mat4f & p_mat )
			{
				_scale = p_mat;
				update();
			}

		  private:
			inline void update() { _transform = _translation * _rotation * _scale; }

			Mat4f _transform   = MAT4F_ID;
			Mat4f _translation = MAT4F_ID;
			Mat4f _rotation	   = MAT4F_ID;
			Mat4f _scale	   = MAT4F_ID;
		};
	} // namespace Math
} // namespace VTX

#endif
