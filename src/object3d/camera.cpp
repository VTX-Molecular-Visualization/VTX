#include "camera.hpp"

namespace VTX
{
	namespace Object3D
	{
		void Camera::moveFront( const float p_delta )
		{
			_position += _front * p_delta;
			_update();
		}

		void Camera::moveLeft( const float p_delta )
		{
			_position += _left * p_delta;
			_update();
		}

		void Camera::moveUp( const float p_delta )
		{
			_position += _up * p_delta;
			_update();
		}

		void Camera::rotateLeft( const float p_delta )
		{
			_yaw = p_delta;
			_update();
		}

		void Camera::rotateUp( const float p_delta )
		{
			_pitch = p_delta;
			_update();
		}

		void Camera::_update()
		{
			Quatf quat = Quatf( glm::vec3( _pitch, _yaw, _roll ) );

			_pitch = 0.f;
			_yaw   = 0.f;
			_roll  = 0.f;

			_rotation		  = quat * _rotation;
			_rotation		  = glm::normalize( _rotation );
			Mat4f rotation	  = glm::mat4_cast( _rotation );
			Mat4f translation = Mat4f( 1.0f );
			translation		  = glm::translate( translation, -_position );
			_viewMatrix		  = rotation * translation;

			_front = Vec3f( _viewMatrix[ 0 ][ 2 ], _viewMatrix[ 1 ][ 2 ], _viewMatrix[ 2 ][ 2 ] );
			_up	   = Vec3f( _viewMatrix[ 0 ][ 1 ], _viewMatrix[ 1 ][ 1 ], _viewMatrix[ 2 ][ 1 ] );
			_left  = Vec3f( _viewMatrix[ 0 ][ 0 ], _viewMatrix[ 1 ][ 0 ], _viewMatrix[ 2 ][ 0 ] );
		}

		void Camera::print() const
		{
			VTX_INFO( "Position: " + glm::to_string( _position ) );
			VTX_INFO( "Rotation: " + glm::to_string( _rotation ) );
			VTX_INFO( "Pitch: " + std::to_string( _pitch ) );
			VTX_INFO( "Yaw: " + std::to_string( _yaw ) );
			VTX_INFO( "Roll: " + std::to_string( _roll ) );
			VTX_INFO( "Front: " + glm::to_string( _front ) );
			VTX_INFO( "Left: " + glm::to_string( _left ) );
			VTX_INFO( "Up: " + glm::to_string( _up ) );
		}

	} // namespace Object3D
} // namespace VTX
