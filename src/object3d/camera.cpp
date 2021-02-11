#include "camera.hpp"
#include "tool/logger.hpp"
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

		void Camera::move( const Vec3f & p_delta )
		{
			_position += _right * p_delta.x;
			_position += _up * p_delta.y;
			_position += _front * p_delta.z;
			_updateViewMatrix();
		}

		void Camera::moveFront( const float p_delta )
		{
			_position += _front * p_delta;
			_updateViewMatrix();
		}

		void Camera::moveRight( const float p_delta )
		{
			_position += _right * p_delta;
			_updateViewMatrix();
		}

		void Camera::moveUp( const float p_delta )
		{
			_position += _up * p_delta;
			_updateViewMatrix();
		}

		void Camera::rotate( const Vec3f & p_delta )
		{
			_rotation = _rotation * Util::Math::eulerToQuaternion( p_delta );
			_updateRotation();
		}

		void Camera::rotatePitch( const float p_delta )
		{
			_rotation = _rotation * Util::Math::eulerToQuaternion( Vec3f( p_delta, 0.f, 0.f ) );
			_updateRotation();
		}

		void Camera::rotateYaw( const float p_delta )
		{
			_rotation = _rotation * Util::Math::eulerToQuaternion( Vec3f( 0.f, p_delta, 0.f ) );
			_updateRotation();
		}

		void Camera::rotateRoll( const float p_delta )
		{
			_rotation = _rotation * Util::Math::eulerToQuaternion( Vec3f( 0.f, 0.f, p_delta ) );
			_updateRotation();
		}

		void Camera::rotateAround( const Quatf & p_rotation, const Vec3f & p_target, const float p_distance )
		{
			_rotation = Util::Math::normalize( _rotation * p_rotation );
			_position = _rotation * Vec3f( 0.0, 0.0, p_distance ) + p_target;
			_updateRotation();
		}

		void Camera::lookAt( const Vec3f & p_target, const Vec3f & p_up )
		{
			_rotation = Util::Math::lookAt( _position, p_target, p_up );
			_updateRotation();
		}

		void Camera::_updateRotation()
		{
			Mat3f rotation = Util::Math::castMat3( _rotation );
			_front		   = rotation * CAMERA_FRONT_DEFAULT;
			_right		   = rotation * CAMERA_RIGHT_DEFAULT;
			_up			   = rotation * CAMERA_UP_DEFAULT;

			_updateViewMatrix();
		}

		void Camera::_updateViewMatrix()
		{
			_viewMatrix = Util::Math::lookAt( _position, _position + _front, _up );
			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		}

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

			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		}

		void Camera::print() const
		{
			VTX_INFO( "Position: " + Util::Math::to_string( _position ) );
			VTX_INFO( "Rotation: " + Util::Math::to_string( _rotation ) );
			VTX_INFO( "Front: " + Util::Math::to_string( _front ) );
			VTX_INFO( "Right: " + Util::Math::to_string( _right ) );
			VTX_INFO( "Up: " + Util::Math::to_string( _up ) );
		}

	} // namespace Object3D
} // namespace VTX
