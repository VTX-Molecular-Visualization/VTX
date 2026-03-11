#ifndef __VTX_UTIL_MATH_TRANSFORM__
#define __VTX_UTIL_MATH_TRANSFORM__

#include "util/math.hpp"
#include "util/types.hpp"

namespace VTX::Util::Math
{
	/**
	 * @brief Defines a 3D transformation (position, rotation, scale).
	 */
	class Transform
	{
	  public:
		/**
		 * @brief Accessors.
		 */
		inline const Vec3f & getPosition() const noexcept { return _position; };
		inline const Quatf & getRotation() const noexcept { return _rotation; };
		inline const Vec3f & getScale() const noexcept { return _scale; };

		/**
		 * @brief Get local axes.
		 */
		inline Vec3f getFront() const noexcept { return Math::toMat3( _rotation ) * FRONT_AXIS; }
		inline Vec3f getRight() const noexcept { return Math::toMat3( _rotation ) * RIGHT_AXIS; }
		inline Vec3f getUp() const noexcept { return Math::toMat3( _rotation ) * UP_AXIS; }

		/**
		 * @brief Reset transformation to identity.
		 */
		inline void reset() noexcept
		{
			_position = VEC3F_ZERO;
			_rotation = QUATF_ID;
			_scale	  = Vec3f( 1.f );
		}

		/**
		 * @brief Position.
		 */
		inline void translate( const Vec3f & p_vec ) noexcept { _position += _rotation * p_vec; }
		inline void setPosition( const float p_x, const float p_y, const float p_z ) noexcept
		{
			_position = Vec3f( p_x, p_y, p_z );
		}
		inline void setPosition( const Vec3f & p_vec ) noexcept { _position = p_vec; }

		/**
		 * @brief Rotation.
		 */
		inline void rotate( const Quatf & p_rotation ) noexcept
		{
			_rotation = Math::normalize( _rotation * p_rotation );
		}
		inline void rotate( const Vec3f & p_eulerAngles ) noexcept
		{
			_rotation = Math::normalize( _rotation * Quatf( p_eulerAngles ) );
		}
		inline void rotate( const float p_angle, const Vec3f & p_axis ) noexcept
		{
			_rotation = Math::normalize( Math::rotate( _rotation, p_angle, p_axis ) );
		}
		inline void rotatePitch( const float p_angle ) noexcept { rotate( RIGHT_AXIS * p_angle ); }
		inline void rotateYaw( const float p_angle ) noexcept { rotate( UP_AXIS * p_angle ); }
		inline void rotateRoll( const float p_angle ) noexcept { rotate( FRONT_AXIS * p_angle ); }
		inline void rotateAround( const Quatf & p_rotation, const Vec3f & p_target, const float p_distance ) noexcept
		{
			_rotation = Math::normalize( _rotation * p_rotation );
			_position = _rotation * Vec3f( 0.f, 0.f, p_distance ) + p_target;
		}
		inline void setRotation( const float p_pitch, const float p_yaw, const float p_roll ) noexcept
		{
			_rotation = Math::normalize(
				Quatf( Vec3f( Math::radians( p_pitch ), Math::radians( p_yaw ), Math::radians( p_roll ) ) )
			);
		}
		inline void setRotation( const Vec3f & p_vec ) noexcept { _rotation = Math::normalize( Quatf( p_vec ) ); }
		inline void setRotation( const Quatf & p_rotation ) noexcept { _rotation = Math::normalize( p_rotation ); }
		inline void setRotationAround(
			const Quatf & p_rotation,
			const Vec3f & p_target,
			const float	  p_distance
		) noexcept
		{
			_rotation = Math::normalize( p_rotation );
			_position = _rotation * Vec3f( 0.f, 0.f, p_distance ) + p_target;
		}
		inline void lookAt( const Vec3f & p_target, const Vec3f & p_up = UP_AXIS ) noexcept
		{
			const Vec3f dir = Math::normalize( p_target - _position );
			_rotation		= Math::quatLookAt( dir, Math::normalize( p_up ) );
		}

		/**
		 * @brief Scale.
		 */
		inline void scale( const Vec3f & p_vec ) noexcept { _scale = _scale * p_vec; }
		inline void setScale( const Vec3f & p_scale ) noexcept { _scale = p_scale; }
		inline void setScale( const float p_scale ) noexcept { _scale = Vec3f( p_scale ); }

		/**
		 * @brief Compute transformation matrix.
		 */
		[[nodiscard]] inline Mat4f computeMatrix() const noexcept
		{
			return Math::translate( _position ) * Math::toMat4( _rotation ) * Math::scale( _scale );
		}

		/**
		 * @brief Compute Euler angles in degrees.
		 */
		[[nodiscard]] inline Vec3f computeEulerAngles() const noexcept
		{
			return Math::degrees( Math::eulerAngles( _rotation ) );
		};

	  private:
		/**
		 * @brief Local translation.
		 */
		Vec3f _position = VEC3F_ZERO;

		/**
		 * @brief Local rotation.
		 */
		Quatf _rotation = QUATF_ID;

		/**
		 * @brief Local scale.
		 */
		Vec3f _scale = VEC3F_XYZ;
	};
} // namespace VTX::Util::Math

#endif
