#include "perspective_camera.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "state/visualization.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Object3D
	{
		PerspectiveCamera::PerspectiveCamera() : Camera() { _updateRotation(); }

		void PerspectiveCamera::fromOrthographic( OrthographicCamera * p_orthographicCamera )
		{
			_screenWidth  = p_orthographicCamera->getScreenWidth();
			_screenHeight = p_orthographicCamera->getScreenHeight();
			_aspectRatio  = p_orthographicCamera->getAspectRatio();
			_near		  = p_orthographicCamera->getNear();
			_far		  = p_orthographicCamera->getFar();
			_fov		  = p_orthographicCamera->getFov();
			_rotation	  = p_orthographicCamera->getRotation();
			_front		  = p_orthographicCamera->getFront();
			_right		  = p_orthographicCamera->getRight();
			_up			  = p_orthographicCamera->getUp();
			_target		  = p_orthographicCamera->getTarget();

			const Vec3f dirTarget = _target - p_orthographicCamera->getPosition();
			_baseDistance		  = Util::Math::length( dirTarget );
			_position = p_orthographicCamera->getPosition() + dirTarget * ( 1.f - p_orthographicCamera->getZoom() );

			_updateViewMatrix();
			_updateProjectionMatrix();
		}

		void PerspectiveCamera::move( const Vec3f & p_delta )
		{
			_position += _right * p_delta.x;
			_position += _up * p_delta.y;
			_position += _front * p_delta.z;
			_updateViewMatrix();
		}

		void PerspectiveCamera::moveFront( const float p_delta )
		{
			_position += _front * p_delta;
			_updateViewMatrix();
		}

		void PerspectiveCamera::moveRight( const float p_delta )
		{
			_position += _right * p_delta;
			_updateViewMatrix();
		}

		void PerspectiveCamera::moveUp( const float p_delta )
		{
			_position += _up * p_delta;
			_updateViewMatrix();
		}

		void PerspectiveCamera::setTarget( const Vec3f & p_target )
		{
			_target		  = p_target;
			_baseDistance = Util::Math::distance( _target, _position );
		}

		void PerspectiveCamera::_updateViewMatrix()
		{
			_viewMatrix = Util::Math::lookAt( _position, _position + _front, _up );
			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		}

		void PerspectiveCamera::_updateProjectionMatrix()
		{
			_projectionMatrix = Util::Math::perspective( Util::Math::radians( _fov ), _aspectRatio, _near, _far );
			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		}
	} // namespace Object3D
} // namespace VTX
