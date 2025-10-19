#include "app/scene/camera.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include <renderer/facade.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::App::Scene
{
	Camera::Camera() :
		_near(
			Util::Math::max(
				1e-1f,
				SETTINGS().get<float>( Settings::Camera::NEAR_CLIP_KEY )
			)
		), // Avoid to little value.
		_far( Util::Math::max( _near, SETTINGS().get<float>( Settings::Camera::FAR_CLIP_KEY ) ) ),
		_fov( SETTINGS().get<float>( Settings::Camera::FOV_KEY ) )
	{
		// Set settings default values.
		auto & cameraProjection = SETTINGS().get<PROJECTION>( Settings::Camera::PROJECTION_KEY );
		setCameraProjection( cameraProjection );
	}

	void Camera::setScreenSize( const size_t p_width, const size_t p_height )
	{
		_screenWidth  = p_width;
		_screenHeight = p_height;
		_aspectRatio  = float( _screenWidth ) / float( _screenHeight );

		//_updateProjectionMatrix();
	}

	void Camera::setNear( const float p_near )
	{
		// Avoid too little value.
		_near = Util::Math::max( 1e-1f, p_near );

		//_updateProjectionMatrix();
	}
	void Camera::setFar( const float p_far )
	{
		// Avoid too little value.
		_far = Util::Math::max( 1e-1f, p_far );

		//_updateProjectionMatrix();
	}

	void Camera::setFov( const float p_fov )
	{
		_fov = p_fov;
		//_updateProjectionMatrix();
	}

	void Camera::setCameraProjection( const PROJECTION & p_projection )
	{
		_projection = p_projection;

		//_updateViewMatrix();
		//_updateProjectionMatrix();
	}

	/*
	void Camera::_updateViewMatrix()
	{
		_viewMatrix = Util::Math::lookAt(
			_transform->getPosition(), _transform->getPosition() + _transform->getFront(), _transform->getUp()
		);

		if ( _projection == PROJECTION::ORTHOGRAPHIC )
			_updateProjectionMatrix();
	}

	void Camera::_updateProjectionMatrix()
	{
		switch ( _projection )
		{
		case PROJECTION::PERSPECTIVE: _computePerspectiveProjectionMatrix(); break;
		case PROJECTION::ORTHOGRAPHIC: _computeOrthographicProjectionMatrix(); break;
		default:
			VTX_WARNING( "Unknown camera projection. Projection computed as Perspective." );
			_computePerspectiveProjectionMatrix();
			break;
		}
	}

	void Camera::_computePerspectiveProjectionMatrix()
	{
		_projectionMatrix = Util::Math::perspective( Util::Math::radians( _fov ), _aspectRatio, _near, _far );
	}
	void Camera::_computeOrthographicProjectionMatrix()
	{
		float top
			= tanf( Util::Math::radians( _fov ) * 0.5f ) * Util::Math::distance( _target, _transform->getPosition() );

		float bottom = -top;
		float right	 = top * _aspectRatio;
		float left	 = -top * _aspectRatio;

		_projectionMatrix = Util::Math::ortho( left, right, bottom, top, _near, _far );
	}
	*/

} // namespace VTX::App::Scene
