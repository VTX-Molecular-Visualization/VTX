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
		const Vec3f & getPosition() const noexcept { return _position; };

		const Quatf & getRotation() const noexcept { return _rotation; };

		const Vec3f & getScale() const noexcept { return _scale; };

		/**
		 * @brief Get local axes.
		 */
		Vec3f getFront() const noexcept { return Math::toMat3( _rotation ) * FRONT_AXIS; }

		Vec3f getRight() const noexcept { return Math::toMat3( _rotation ) * RIGHT_AXIS; }

		Vec3f getUp() const noexcept { return Math::toMat3( _rotation ) * UP_AXIS; }

		/**
		 * @brief Reset transformation to identity.
		 */
		void reset() noexcept
		{
			_position = VEC3F_ZERO;
			_rotation = QUATF_ID;
			_scale	  = Vec3f( 1.f );
		}

		/**
		 * @brief Position.
		 */
		void translate( const Vec3f & p_vec ) noexcept { _position += _rotation * p_vec; }

		void setPosition( const float p_x, const float p_y, const float p_z ) noexcept
		{ _position = Vec3f( p_x, p_y, p_z ); }

		void setPosition( const Vec3f & p_vec ) noexcept { _position = p_vec; }

		/**
		 * @brief Rotation.
		 */
		void rotate( const Quatf & p_rotation ) noexcept { _rotation = Math::normalize( _rotation * p_rotation ); }

		void rotate( const Vec3f & p_eulerAngles ) noexcept
		{ _rotation = Math::normalize( _rotation * Quatf( p_eulerAngles ) ); }

		void rotate( const float p_angle, const Vec3f & p_axis ) noexcept
		{ _rotation = Math::normalize( Math::rotate( _rotation, p_angle, p_axis ) ); }

		void rotatePitch( const float p_angle ) noexcept { rotate( RIGHT_AXIS * p_angle ); }

		void rotateYaw( const float p_angle ) noexcept { rotate( UP_AXIS * p_angle ); }

		void rotateRoll( const float p_angle ) noexcept { rotate( FRONT_AXIS * p_angle ); }

		void rotateAround( const Quatf & p_rotation, const Vec3f & p_target, const float p_distance ) noexcept
		{
			_rotation = Math::normalize( _rotation * p_rotation );
			_position = _rotation * Vec3f( 0.f, 0.f, p_distance ) + p_target;
		}

		void setRotation( const float p_pitch, const float p_yaw, const float p_roll ) noexcept
		{
			_rotation = Math::normalize(
				Quatf( Vec3f( Math::radians( p_pitch ), Math::radians( p_yaw ), Math::radians( p_roll ) ) )
			);
		}

		void setRotation( const Vec3f & p_vec ) noexcept { _rotation = Math::normalize( Quatf( p_vec ) ); }

		void setRotation( const Quatf & p_rotation ) noexcept { _rotation = Math::normalize( p_rotation ); }

		void setRotationAround( const Quatf & p_rotation, const Vec3f & p_target, const float p_distance ) noexcept
		{
			_rotation = Math::normalize( p_rotation );
			_position = _rotation * Vec3f( 0.f, 0.f, p_distance ) + p_target;
		}

		void lookAt( const Vec3f & p_target, const Vec3f & p_up = UP_AXIS ) noexcept
		{
			const Vec3f dir = Math::normalize( p_target - _position );
			_rotation		= Math::quatLookAt( dir, Math::normalize( p_up ) );
		}

		/**
		 * @brief Scale.
		 */
		void scale( const Vec3f & p_vec ) noexcept { _scale = _scale * p_vec; }

		void setScale( const Vec3f & p_scale ) noexcept { _scale = p_scale; }

		void setScale( const float p_scale ) noexcept { _scale = Vec3f( p_scale ); }

		/**
		 * @brief Compute transformation matrix.
		 */
		[[nodiscard]] Mat4f computeMatrix() const noexcept
		{ return Math::translate( _position ) * Math::toMat4( _rotation ) * Math::scale( _scale ); }

		/**
		 * @brief Compute Euler angles in degrees.
		 */
		[[nodiscard]] Vec3f computeEulerAngles() const noexcept
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
