#include "camera.hpp"
#include "setting.hpp"
#include "tool/logger.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Object3D
	{
		Camera::Camera() :
			_near( Util::Math::max( 1e-1f, VTX_SETTING().getCameraNearClip() ) ), // Avoid to little value.
			_far( Util::Math::max( _near, VTX_SETTING().getCameraFarClip() ) ), _fov( VTX_SETTING().getCameraFOV() )
		{
		}

		void Camera::setFrontRightUp( const Vec3f & p_front, const Vec3f & p_right, const Vec3f & p_up )
		{
			_front = p_front;
			_right = p_right;
			_up	   = p_up;
			_updateViewMatrix();
		}

		void Camera::setNear( const float p_near )
		{
			// Avoid too little value.
			_near = Util::Math::max( 1e-1f, p_near );
			_updateProjectionMatrix();
		}
		void Camera::setFar( const float p_far )
		{
			// Avoid too little value.
			_far = Util::Math::max( 1e-1f, p_far );
			_updateProjectionMatrix();
		}

		void Camera::setFov( const float p_fov )
		{
			_fov = p_fov;
			_updateProjectionMatrix();
		}

		void Camera::setTarget( const Vec3f & p_target ) { _target = p_target; }

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

		void Camera::setRotationAround( const Quatf & p_rotation, const Vec3f & p_target, const float p_distance )
		{
			_rotation = Util::Math::normalize( p_rotation );
			_position = _rotation * Vec3f( 0.f, 0.f, p_distance ) + p_target;
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

		void Camera::reset( const Vec3f & p_defaultPosition )
		{
			_position = p_defaultPosition;
			_rotation = QUATF_ID;
			_updateRotation();
		}

		void Camera::_updateRotation()
		{
			const Mat3f rotation = Util::Math::castMat3( _rotation );
			_front				 = rotation * CAMERA_FRONT_DEFAULT;
			_right				 = rotation * CAMERA_RIGHT_DEFAULT;
			_up					 = rotation * CAMERA_UP_DEFAULT;

			_updateViewMatrix();
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
