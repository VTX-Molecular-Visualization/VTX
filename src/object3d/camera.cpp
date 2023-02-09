#include "camera.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "tool/logger.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Object3D
	{
		Camera::Camera() :
			_near( Util::Math::max( 1e-1f, VTX_RENDER_EFFECT().getCameraNearClip() ) ), // Avoid to little value.
			_far( Util::Math::max( _near, VTX_RENDER_EFFECT().getCameraFarClip() ) ),
			_fov( VTX_RENDER_EFFECT().getCameraFOV() ), _isPerspective( VTX_RENDER_EFFECT().isPerspectiveProjection() )
		{
			_updateRotation();
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

		void Camera::setPerspective( const bool p_perspective )
		{
			_isPerspective = p_perspective;
			_updateProjectionMatrix();
		}

		void Camera::toggle()
		{
			_isPerspective = !_isPerspective;
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

		void Camera::setZoom( const float p_zoom ) { _zoom = Util::Math::max( p_zoom, 1e-3f ); }
		void Camera::zoom( const float p_delta ) { _zoom = Util::Math::max( _zoom + p_delta, 1e-3f ); }

		void Camera::reset( const Vec3f & p_defaultPosition )
		{
			_position = p_defaultPosition;
			_rotation = QUATF_ID;
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
			if ( _isPerspective )
			{
				_projectionMatrix = Util::Math::perspective( Util::Math::radians( _fov ), _aspectRatio, _near, _far );
			}
			else
			{
				_projectionMatrix = Util::Math::ortho( -20.f, 20.f, -20.f, 20.f, _near, _far );
			}

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
