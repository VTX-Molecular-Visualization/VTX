#include "app/component/render/camera.hpp"
#include "app/application/system/settings_system.hpp"
#include "app/internal/application/settings.hpp"
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::App::Component::Render
{
	using namespace App::Internal::Application::Settings::Camera;

	Camera::Camera() :
		_near( Util::Math::max(
			1e-1f,
			SETTINGS().get<float>( NEAR_CLIP_KEY )
		) ), // Avoid to little value.
		_far( Util::Math::max( _near, SETTINGS().get<float>( FAR_CLIP_KEY ) ) ),
		_fov( SETTINGS().get<float>( FOV_KEY ) )
	{
		const CAMERA_PROJECTION & cameraProjection = SETTINGS().get<CAMERA_PROJECTION>( PROJECTION_KEY );

		_projection = cameraProjection;
	}

	void Camera::init()
	{
		assert( MAIN_REGISTRY().hasComponent<Component::Scene::Transform>( *this ) );

		Component::Scene::Transform & transformComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::Transform>( *this );

		_transform = &transformComponent;

		_transform->onTransform += [ this ]( const Util::Math::Transform & ) { _updateViewMatrix(); };

		_updateViewMatrix();
		_updateProjectionMatrix();
	}

	void Camera::setScreenSize( const size_t p_width, const size_t p_height )
	{
		_screenWidth  = p_width;
		_screenHeight = p_height;
		_aspectRatio  = float( _screenWidth ) / float( _screenHeight );

		_updateProjectionMatrix();
	}

	void Camera::setNear( const float p_near )
	{
		// Avoid too little value.
		_near = Util::Math::max( 1e-1f, p_near );

		_updateProjectionMatrix();
		onClipInfosChange( _near, _far );
	}
	void Camera::setFar( const float p_far )
	{
		// Avoid too little value.
		_far = Util::Math::max( 1e-1f, p_far );

		_updateProjectionMatrix();
		onClipInfosChange( _near, _far );
	}

	void Camera::setFov( const float p_fov )
	{
		_fov = p_fov;
		_updateProjectionMatrix();
	}

	void Camera::setTargetWorld( const Vec3f & p_target )
	{
		if ( _targetIsLocal )
		{
			_target = getTransform().getPosition() - _target;
		}
		else
		{
			_target = p_target;
		}
	}
	void Camera::setTargetLocal( const Vec3f & p_target )
	{
		if ( _targetIsLocal )
		{
			_target = p_target;
		}
		else
		{
			_target = getTransform().getPosition() + _target;
		}
	}
	void Camera::attachTarget()
	{
		if ( !_targetIsLocal )
		{
			_targetIsLocal = true;
			_target		   = getTransform().getPosition() - _target;
		}
	}
	void Camera::detachTarget()
	{
		if ( _targetIsLocal )
		{
			_targetIsLocal = false;
			_target = getTransform().getPosition() + ( getTransform().getFront() * Util::Math::length( _target ) );
		}
	}
	float Camera::getDistanceToTarget() const { return Util::Math::distance( _transform->getPosition(), _target ); }

	void Camera::reset( const Vec3f & p_defaultPosition ) { _transform->set( p_defaultPosition, QUATF_ID ); }

	void Camera::setCameraProjection( const CAMERA_PROJECTION & p_projection )
	{
		_projection = p_projection;

		onProjectionChange( _projection );

		_updateViewMatrix();
		_updateProjectionMatrix();
	}

	void Camera::_updateViewMatrix()
	{
		_viewMatrix = Util::Math::lookAt(
			_transform->getPosition(), _transform->getPosition() + _transform->getFront(), _transform->getUp()
		);

		onMatrixViewChange( _viewMatrix );

		if ( _projection == CAMERA_PROJECTION::ORTHOGRAPHIC )
			_updateProjectionMatrix();
	}

	void Camera::_updateProjectionMatrix()
	{
		switch ( _projection )
		{
		case CAMERA_PROJECTION::PERSPECTIVE: _computePerspectiveProjectionMatrix(); break;
		case CAMERA_PROJECTION::ORTHOGRAPHIC: _computeOrthographicProjectionMatrix(); break;
		default:
			VTX_WARNING( "Unknown camera projection. Projection computed as Perspective." );
			_computePerspectiveProjectionMatrix();
			break;
		}

		onMatrixProjectionChange( _projectionMatrix );
		// App::Old::APP::MASK |= App::Old::Render::VTX_MASK_CAMERA_UPDATED;
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

	void Camera::print() const
	{
		VTX_INFO( "Position: {}", Util::Math::to_string_fmt( _transform->getPosition() ) );
		VTX_INFO( "Rotation: {}", Util::Math::to_string_fmt( _transform->getRotation() ) );
		VTX_INFO( "Front: {}", Util::Math::to_string_fmt( _transform->getFront() ) );
		VTX_INFO( "Right: {}", Util::Math::to_string_fmt( _transform->getRight() ) );
		VTX_INFO( "Up: {}", Util::Math::to_string_fmt( _transform->getUp() ) );

		VTX_INFO( "Projection: ", int( _projection ) );
	}

} // namespace VTX::App::Component::Render
