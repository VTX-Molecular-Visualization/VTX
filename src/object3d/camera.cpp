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

		void Camera::rotate( const Vec3d & p_delta )
		{
			_eulerAngles = p_delta;
			_rotation	 = _rotation * Util::Math::eulerToQuaternion( _eulerAngles );
			_updateRotation();
		}

		void Camera::rotatePitch( const float p_delta )
		{
			_eulerAngles.x = -p_delta;
			_rotation	   = _rotation * Util::Math::eulerToQuaternion( _eulerAngles );
			_updateRotation();
		}

		void Camera::rotateYaw( const float p_delta )
		{
			_eulerAngles.y = -p_delta;
			_rotation	   = _rotation * Util::Math::eulerToQuaternion( _eulerAngles );
			_updateRotation();
		}

		void Camera::rotateRoll( const float p_delta )
		{
			_eulerAngles.z = p_delta;
			_rotation	   = _rotation * Util::Math::eulerToQuaternion( _eulerAngles );
			_updateRotation();
		}

		void Camera::rotateAround( const Vec3f & p_target, const Vec3f & p_axis, const float p_delta )
		{
			//_rotation = Quatd( Util::Math::lookAt( _position, p_target, VEC3F_Z ) );
			_updateRotation();
		}

		void Camera::_updateRotation()
		{
			_rotation	   = Util::Math::normalize( _rotation );
			Mat3d rotation = Util::Math::castMat3( _rotation );
			_front		   = rotation * -VEC3F_Z;
			_left		   = rotation * -VEC3F_X;
			_up			   = rotation * VEC3F_Y;
			_updateViewMatrix();
			_eulerAngles = VEC3F_ZERO;
		}

		void Camera::_updateViewMatrix() { _viewMatrix = Util::Math::lookAt( _position, _position + _front, _up ); }

		void Camera::_updateProjectionMatrix()
		{
			_projectionMatrix
				= Util::Math::perspective( Util::Math::radians( _fov ), _screenWidth / _screenHeight, _near, _far );
		}

		void Camera::print() const
		{
			VTX_INFO( "Position: " + Util::Math::to_string( _position ) );
			VTX_INFO( "Rotation: " + Util::Math::to_string( _rotation ) );
			VTX_INFO( "Euler angles: " + Util::Math::to_string( _eulerAngles ) );
			VTX_INFO( "Front: " + Util::Math::to_string( _front ) );
			VTX_INFO( "Left: " + Util::Math::to_string( _left ) );
			VTX_INFO( "Up: " + Util::Math::to_string( _up ) );
		}

	} // namespace Object3D
} // namespace VTX
