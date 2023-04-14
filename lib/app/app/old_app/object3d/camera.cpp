#include "camera.hpp"
#include "old_app/setting.hpp"
#include <util/logger.hpp>
#include "old_app/vtx_app.hpp"

namespace VTX
{
	namespace Object3D
	{
		Camera::Camera() :
			_near( Util::Math::max( 1e-1f, VTX_SETTING().getCameraNearClip() ) ), // Avoid to little value.
			_far( Util::Math::max( _near, VTX_SETTING().getCameraFarClip() ) ), _fov( VTX_SETTING().getCameraFOV() )
		{
			_updateRotation();

			const CameraProjection & cameraProjection
				= VTX_SETTING().getCameraPerspective() ? CameraProjection::PERSPECTIVE : CameraProjection::ORTHOGRAPHIC;
			setCameraProjection( cameraProjection );
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

		void  Camera::setTarget( const Vec3f & p_target ) { _target = p_target; }
		float Camera::getDistanceToTarget() const { return Util::Math::distance( getPosition(), _target ); }

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

		void Camera::setCameraProjection( const CameraProjection & p_projection )
		{
			_projection = p_projection;

			_updateViewMatrix();
			_updateProjectionMatrix();
		}

		void Camera::_updateViewMatrix()
		{
			_viewMatrix = Util::Math::lookAt( _position, _position + _front, _up );

			if ( _projection == CameraProjection::ORTHOGRAPHIC )
				_updateProjectionMatrix();

			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		}

		void Camera::_updateProjectionMatrix()
		{
			switch ( _projection )
			{
			case CameraProjection::PERSPECTIVE: _computePerspectiveProjectionMatrix(); break;
			case CameraProjection::ORTHOGRAPHIC: _computeOrthographicProjectionMatrix(); break;
			default:
				VTX_WARNING( "Unknown camera projection. Projection computed as Perspective." );
				_computePerspectiveProjectionMatrix();
				break;
			}

			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		}

		void Camera::_computePerspectiveProjectionMatrix()
		{
			_projectionMatrix = Util::Math::perspective( Util::Math::radians( _fov ), _aspectRatio, _near, _far );
		}
		void Camera::_computeOrthographicProjectionMatrix()
		{
			float top = tanf( Util::Math::radians( _fov ) * 0.5f ) * Util::Math::distance( _target, _position );

			float bottom = -top;
			float right	 = top * _aspectRatio;
			float left	 = -top * _aspectRatio;

			_projectionMatrix = Util::Math::ortho( left, right, bottom, top, _near, _far );
		}

		void Camera::print() const
		{
			VTX_INFO( "Position: " + Util::Math::to_string_fmt( _position ) );
			VTX_INFO( "Rotation: " + Util::Math::to_string_fmt( _rotation ) );
			VTX_INFO( "Front: " + Util::Math::to_string_fmt( _front ) );
			VTX_INFO( "Right: " + Util::Math::to_string_fmt( _right ) );
			VTX_INFO( "Up: " + Util::Math::to_string_fmt( _up ) );

			VTX_INFO( "Projection: " + std::to_string( int( _projection ) ) );
		}

	} // namespace Object3D
} // namespace VTX
