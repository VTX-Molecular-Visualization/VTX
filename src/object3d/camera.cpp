#include "camera.hpp"

namespace VTX
{
	namespace Object3D
	{
		void Camera::move( const Vec3f & p_delta )
		{
			_position += _left * p_delta.x;
			_position += _up * p_delta.y;
			_position += _front * p_delta.z;
			_updateViewMatrix();
		}

		void Camera::moveFront( const float p_delta )
		{
			_position += _front * p_delta;
			_updateViewMatrix();
		}

		void Camera::moveLeft( const float p_delta )
		{
			_position += _left * p_delta;
			_updateViewMatrix();
		}

		void Camera::moveUp( const float p_delta )
		{
			_position += _up * p_delta;
			_updateViewMatrix();
		}

		void Camera::rotateLeft( const float p_delta )
		{
			_eulerAngles.y -= p_delta;
			_rotation = Util::Math::eulerToQuaternion( _eulerAngles );
			_updateRotation();
		}

		void Camera::rotateUp( const float p_delta )
		{
			_eulerAngles.x -= p_delta;
			_rotation = Util::Math::eulerToQuaternion( _eulerAngles );
			_updateRotation();
		}

		void Camera::_updateRotation()
		{
			Mat3f rotation = glm::mat3_cast( _rotation );
			_front		   = rotation * -VEC3F_Z;
			_left		   = rotation * -VEC3F_X;
			_up			   = rotation * VEC3F_Y;
			_updateViewMatrix();
		}

		void Camera::_updateViewMatrix() { _viewMatrix = glm::lookAt( _position, _position + _front, _up ); }

		void Camera::print() const
		{
			VTX_INFO( "Position: " + glm::to_string( _position ) );
			VTX_INFO( "Rotation: " + glm::to_string( _rotation ) );
			VTX_INFO( "Euler angles: " + glm::to_string( _eulerAngles ) );
			VTX_INFO( "Front: " + glm::to_string( _front ) );
			VTX_INFO( "Left: " + glm::to_string( _left ) );
			VTX_INFO( "Up: " + glm::to_string( _up ) );
		}

	} // namespace Object3D
} // namespace VTX
