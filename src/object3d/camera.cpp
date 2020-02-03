#include "camera.hpp"

namespace VTX
{
	namespace Object3D
	{
		void Camera::moveFront( const float p_delta )
		{
			_eye += _front * p_delta;
			_update();
		}

		void Camera::moveLeft( const float p_delta )
		{
			_eye += _left * p_delta;
			_update();
		}

		void Camera::moveUp( const float p_delta )
		{
			_eye += _up * p_delta;
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

			_quat			  = quat * _quat;
			_quat			  = glm::normalize( _quat );
			Mat4f rotation	  = glm::mat4_cast( _quat );
			Mat4f translation = Mat4f( 1.0f );
			translation		  = glm::translate( translation, -_eye );
			_viewMatrix		  = rotation * translation;

			_front = Vec3f( _viewMatrix[ 0 ][ 2 ], _viewMatrix[ 1 ][ 2 ], _viewMatrix[ 2 ][ 2 ] );
			_up	   = Vec3f( _viewMatrix[ 0 ][ 1 ], _viewMatrix[ 1 ][ 1 ], _viewMatrix[ 2 ][ 1 ] );
			_left  = Vec3f( _viewMatrix[ 0 ][ 0 ], _viewMatrix[ 1 ][ 0 ], _viewMatrix[ 2 ][ 0 ] );

			/*
			_front		= rotation * VEC3F_Z;
			_left		= rotation * VEC3F_X;
			_up			= rotation * VEC3F_Y;
			_viewMatrix = glm::lookAt( _eye, _eye + _front, _up );
			*/
		}

	} // namespace Object3D
} // namespace VTX
