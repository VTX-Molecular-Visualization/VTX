#include "orthographic_camera.hpp"
#include "perspective_camera.hpp"
#include "state/visualization.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Object3D
	{
		OrthographicCamera::OrthographicCamera() : Camera() { _updateRotation(); }

		void OrthographicCamera::fromPerspective( PerspectiveCamera * p_perspectiveCamera )
		{
			if ( p_perspectiveCamera->getBaseDistance() == 0.f )
				p_perspectiveCamera->setBaseDistance(
					Util::Math::distance( p_perspectiveCamera->getPosition(), p_perspectiveCamera->getTarget() ) );

			_screenWidth  = p_perspectiveCamera->getScreenWidth();
			_screenHeight = p_perspectiveCamera->getScreenHeight();
			_aspectRatio  = p_perspectiveCamera->getAspectRatio();
			_near		  = p_perspectiveCamera->getNear();
			_far		  = p_perspectiveCamera->getFar();
			_fov		  = p_perspectiveCamera->getFov();
			_rotation	  = p_perspectiveCamera->getRotation();
			_front		  = p_perspectiveCamera->getFront();
			_right		  = p_perspectiveCamera->getRight();
			_up			  = p_perspectiveCamera->getUp();
			_target		  = p_perspectiveCamera->getTarget();

			_zoom = Util::Math::distance( p_perspectiveCamera->getPosition(), _target )
					/ p_perspectiveCamera->getBaseDistance();

			const Vec3f dirPos = Util::Math::normalize( p_perspectiveCamera->getPosition() - _target );
			_position		   = _target + dirPos * p_perspectiveCamera->getBaseDistance();

			_updateViewMatrix();
		}

		void OrthographicCamera::_updateViewMatrix()
		{
			_viewMatrix = Util::Math::lookAt( _position, _position + _front, _up );
			_updateProjectionMatrix();
			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		}

		void OrthographicCamera::_updateProjectionMatrix()
		{
			float top = tanf( Util::Math::radians( _fov ) * 0.5f ) * Util::Math::distance( _target, _position ) * _zoom;

			float bottom = -top;
			float right	 = top * _aspectRatio;
			float left	 = -top * _aspectRatio;

			_projectionMatrix = Util::Math::ortho( left, right, bottom, top, _near, _far );

			VTXApp::get().MASK |= VTX_MASK_CAMERA_UPDATED;
		}
	} // namespace Object3D
} // namespace VTX
