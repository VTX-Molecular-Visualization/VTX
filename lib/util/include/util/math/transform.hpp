#ifndef __VTX_UTIL_MATH_TRANSFORM__
#define __VTX_UTIL_MATH_TRANSFORM__

#include "util/math.hpp"
#include "util/types.hpp"

namespace VTX::Util::Math
{
	class Transform
	{
	  public:
		Transform( const Vec3f & p_position, const Quatf & p_rotation, const Vec3f & p_scale ) :
			_position( p_position ), _rotation( p_rotation ), _scale( p_scale ), _isDirty( true )
		{
		}

		inline const Mat4f & get() const
		{
			if ( _isDirty )
			{
				_update();
			}
			return _transform;
		}

		inline const Vec3f & getPosition() const { return _position; };
		inline const Quatf & getRotation() const { return _rotation; };
		inline const Vec3f & getScale() const { return _scale; };

		inline Vec3f getFront() const { return Math::mat3_cast( _rotation ) * FRONT_AXIS; }
		inline Vec3f getRight() const { return Math::mat3_cast( _rotation ) * RIGHT_AXIS; }
		inline Vec3f getUp() const { return Math::mat3_cast( _rotation ) * UP_AXIS; }

		inline const Vec3f & getEulerAngles() const
		{
			if ( _isDirty )
			{
				_update();
			}
			return _eulerAngles;
		};

		inline void reset()
		{
			_transform = MAT4F_ID;
			_position  = VEC3F_ZERO;
			_rotation  = QUATF_ID;
			_scale	   = Vec3f( 1.f );
			_isDirty   = true;
		}

		inline void translate( const Vec3f & p_vec )
		{
			_position += p_vec;
			_isDirty = true;
		}

		inline void setPosition( const float p_x, const float p_y, const float p_z )
		{
			_position = Vec3f( p_x, p_y, p_z );
			_isDirty  = true;
		}

		inline void setPosition( const Vec3f & p_vec )
		{
			_position = p_vec;
			_isDirty  = true;
		}

		inline void rotate( const Quatf & p_rotation )
		{
			_rotation *= p_rotation;
			_isDirty = true;
		}

		inline void rotate( const Vec3f & p_eulerAngles )
		{
			_rotation *= Quatf( p_eulerAngles );
			_isDirty = true;
		}

		inline void rotate( const float p_angle, const Vec3f & p_axis )
		{
			_rotation = Math::rotate( _rotation, p_angle, p_axis );
			_isDirty  = true;
		}

		inline void rotateAround( const Quatf & p_rotation, const Vec3f & p_target, const float p_distance )
		{
			_rotation = Math::normalize( _rotation * p_rotation );
			_position = _rotation * Vec3f( 0.f, 0.f, p_distance ) + p_target;
			_isDirty  = true;
		}

		inline void setRotation( const float p_pitch, const float p_yaw, const float p_roll )
		{
			_rotation = Quatf( Vec3f( Math::radians( p_pitch ), Math::radians( p_yaw ), Math::radians( p_roll ) ) );
			_isDirty  = true;
		}

		inline void setRotation( const Vec3f & p_vec )
		{
			_rotation = Quatf( p_vec );
			_isDirty  = true;
		}

		inline void setRotation( const Quatf & p_rotation )
		{
			_rotation = p_rotation;
			_isDirty  = true;
		}

		inline void setRotationAround( const Quatf & p_rotation, const Vec3f & p_target, const float p_distance )
		{
			_rotation = Math::normalize( p_rotation );
			_position = _rotation * Vec3f( 0.f, 0.f, p_distance ) + p_target;
			_isDirty  = true;
		}

		inline void setRotation( const Mat4f & p_mat )
		{
			_rotation = p_mat;
			_isDirty  = true;
		}

		inline void scale( const Vec3f & p_vec )
		{
			_scale	 = _scale * p_vec;
			_isDirty = true;
		}

		inline void setScale( const Vec3f & p_scale )
		{
			_scale	 = p_scale;
			_isDirty = true;
		}

		inline void setScale( const float p_scale )
		{
			_scale	 = Vec3f( p_scale );
			_isDirty = true;
		}

		inline void lookAt( const Vec3f & p_target, const Vec3f & p_up )
		{
			_rotation = Math::lookAt( _position, p_target, p_up );
			_isDirty  = true;
		}

	  private:
		/**
		 * @brief Computed transform.
		 */
		mutable Mat4f _transform = MAT4F_ID;

		/**
		 * @brief Computed euler angles (in degrees).
		 */
		mutable Vec3f _eulerAngles = VEC3F_ZERO;

		/**
		 * @brief Local translation, rotation and scale matrices.
		 */
		Vec3f _position = VEC3F_ZERO;
		Quatf _rotation = QUATF_ID;
		Vec3f _scale	= Vec3f( 1.f );

		/**
		 * @brief Cache vars.
		 */
		mutable bool  _isDirty			  = true;
		mutable Vec3f _internalEulerCache = VEC3F_ZERO;

		/**
		 * @brief Updates the internal transformation matrix by combining translation, rotation, and scale.
		 */
		inline void _update() const
		{
			const Mat4f T = Math::translate( MAT4F_ID, _position );
			const Mat4f R = Mat4f( _rotation );
			const Mat4f S = Math::scale( MAT4F_ID, _scale );
			_transform	  = T * R * S;
			_eulerAngles  = Math::degrees( Math::eulerAngles( _rotation ) );
			_isDirty	  = false;
		}
	};
} // namespace VTX::Util::Math

#endif
