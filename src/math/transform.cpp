#include "transform.hpp"

namespace VTX
{
	namespace Math
	{
		void Transform::reset()
		{
			_transform	 = MAT4F_ID;
			_translation = MAT4F_ID;
			_rotation	 = MAT4F_ID;
			_scale		 = MAT4F_ID;
		}

		void Transform::translate( const Vec3f & p_vec )
		{
			_translation = glm::translate( _translation, p_vec );
			update();
		}

		void Transform::setTranslation( const Mat4f & p_mat )
		{
			_translation = p_mat;
			update();
		}

		void Transform::rotate( const float p_angle, const Vec3f & p_axis )
		{
			_rotation = glm::rotate( _rotation, p_angle, p_axis );
			update();
		}

		void Transform::Transform::setRotation( const Mat4f & p_mat )
		{
			_rotation = p_mat;
			update();
		}

		void Transform::scale( const Vec3f & p_vec )
		{
			_scale = glm::scale( _scale, p_vec );
			update();
		}

		void Transform::setScale( const Mat4f & p_mat )
		{
			_scale = p_mat;
			update();
		}

	} // namespace Math
} // namespace VTX
