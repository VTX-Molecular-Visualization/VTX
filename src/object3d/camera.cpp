#include "camera.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Object3D
	{
		// TODO? (20_05_27): _near, _far, _fov must be initialized in cpp because setting.hpp cannot be included in hpp
		Camera::Camera() :
			_near( Util::Math::max( 1e-1f, VTX_SETTING().cameraNear ) ), // Avoid to little value.
			_far( Util::Math::max( _near, VTX_SETTING().cameraFar ) ), _fov( VTX_SETTING().cameraFov ),
			_isPerspective( VTX_SETTING().cameraPerspective )
		{
			_updateRotation();
		}

		void Camera::move( const Vec3d & p_delta )
		{
			_position += _left * p_delta.x;
			_position += _up * p_delta.y;
			_position += _front * p_delta.z;
			_updateViewMatrix();
		}

		void Camera::moveFront( const double p_delta )
		{
			_position += _front * p_delta;
			_updateViewMatrix();
		}

		void Camera::moveLeft( const double p_delta )
		{
			_position += _left * p_delta;
			_updateViewMatrix();
		}

		void Camera::moveUp( const double p_delta )
		{
			_position += _up * p_delta;
			_updateViewMatrix();
		}

		void Camera::rotate( const Vec3d & p_delta )
		{
			_rotation = _rotation * Util::Math::eulerToQuaternion( p_delta );
			_updateRotation();
		}

		void Camera::rotatePitch( const double p_delta )
		{
			_rotation = _rotation * Util::Math::eulerToQuaternion( Vec3d( -p_delta, 0.0, 0.0 ) );
			_updateRotation();
		}

		void Camera::rotateYaw( const double p_delta )
		{
			_rotation = _rotation * Util::Math::eulerToQuaternion( Vec3d( 0.0, -p_delta, 0.0 ) );
			_updateRotation();
		}

		void Camera::rotateRoll( const double p_delta )
		{
			_rotation = _rotation * Util::Math::eulerToQuaternion( Vec3d( 0.0, 0.0, p_delta ) );
			_updateRotation();
		}

		void Camera::rotateAround( const Quatd & p_rotation, const Vec3d & p_target, const double p_distance )
		{
			_rotation = _rotation * p_rotation;
			_position = _rotation * Vec3d( 0.0, 0.0, p_distance ) + p_target;
			_updateRotation();
		}

		void Camera::_updateRotation()
		{
			Mat3d rotation = Util::Math::castMat3( _rotation );
			_front		   = rotation * -VEC3F_Z;
			_left		   = rotation * -VEC3F_X;
			_up			   = rotation * VEC3F_Y;

			_updateViewMatrix();
		}

		void Camera::_updateViewMatrix() { _viewMatrix = Util::Math::lookAt( _position, _position + _front, _up ); }

		void Camera::_updateProjectionMatrix()
		{
			// if ( _isPerspective )
			//{
			_projectionMatrix = Util::Math::perspective( Util::Math::radians( _fov ), _aspectRatio, _near, _far );
			//}
			// else // Ortho.
			//{
			//	// TODO !
			//}
		}

		void Camera::print() const
		{
			VTX_INFO( "Position: " + Util::Math::to_string( _position ) );
			VTX_INFO( "Rotation: " + Util::Math::to_string( _rotation ) );
			VTX_INFO( "Left: " + Util::Math::to_string( _left ) );
			VTX_INFO( "Up: " + Util::Math::to_string( _up ) );
		}

	} // namespace Object3D
} // namespace VTX
