#ifndef __VTX_RENDERER_BENCH_CAMERA__
#define __VTX_RENDERER_BENCH_CAMERA__

#include <algorithm>
#include <renderer/camera.hpp>
#include <util/constants.hpp>
#include <util/math.hpp>
#include <util/math/transform.hpp>
#include <util/types.hpp>

namespace VTX::Bench
{
	class CameraController
	{
	  public:
		inline static constexpr float VELOCITY_TRANSLATION_DEFAULT = 150.f;
		inline static constexpr float VELOCITY_TRANSLATION_MIN	   = 10.f;
		inline static constexpr float VELOCITY_TRANSLATION_MAX	   = 500.f;

		inline static constexpr float VELOCITY_ROTATION_DEFAULT = 1.5f;
		inline static constexpr float VELOCITY_ROTATION_MIN	   = 0.1f;
		inline static constexpr float VELOCITY_ROTATION_MAX	   = 10.f;

		inline static constexpr float VELOCITY_ZOOM_DEFAULT = 300.f;
		inline static constexpr float VELOCITY_ZOOM_MIN	   = 10.f;
		inline static constexpr float VELOCITY_ZOOM_MAX	   = 2000.f;

		CameraController( const size_t p_width, const size_t p_height ) : _camera { .screenWidth = p_width, .screenHeight = p_height }
		{
			_transform.setPosition( 0.f, 0.f, 250.f );
			_updateTarget();
		}

		inline void translate( const Vec3f & p_delta )
		{
			_transform.translate( p_delta * _velocityTranslation );
			_updateTarget();
		}

		inline void rotate( const Vec3f & p_delta )
		{
			_transform.rotate( p_delta * _velocityRotation );
			_updateTarget();
		}

		inline void zoom( const float p_delta )
		{
			_transform.translate( FRONT_AXIS * ( p_delta * _velocityZoom ) );
			_updateTarget();
		}

		inline void resize( const size_t p_width, const size_t p_height )
		{
			_camera.screenWidth	 = p_width;
			_camera.screenHeight = p_height;
		}

		inline const Renderer::Camera & getCamera() const noexcept { return _camera; }
		inline const Vec3f &			  getPosition() const noexcept { return _transform.getPosition(); }
		inline float					  getNear() const noexcept { return _camera.near; }
		inline float					  getFar() const noexcept { return _camera.far; }
		inline float					  getFov() const noexcept { return _camera.fov; }
		inline float					  getVelocityTranslation() const noexcept { return _velocityTranslation; }
		inline float					  getVelocityRotation() const noexcept { return _velocityRotation; }
		inline float					  getVelocityZoom() const noexcept { return _velocityZoom; }

		inline void setNear( const float p_value ) noexcept
		{
			_camera.near = std::clamp( p_value, Renderer::NEAR_CLIP_MIN, Renderer::NEAR_CLIP_MAX );
		}
		inline void setFar( const float p_value ) noexcept
		{
			_camera.far = std::clamp( p_value, Renderer::FAR_CLIP_MIN, Renderer::FAR_CLIP_MAX );
		}
		inline void setFov( const float p_value ) noexcept
		{
			_camera.fov = std::clamp( p_value, Renderer::FOV_MIN, Renderer::FOV_MAX );
		}

		inline void setVelocityTranslation( const float p_value ) noexcept
		{
			_velocityTranslation = std::clamp( p_value, VELOCITY_TRANSLATION_MIN, VELOCITY_TRANSLATION_MAX );
		}
		inline void setVelocityRotation( const float p_value ) noexcept
		{
			_velocityRotation = std::clamp( p_value, VELOCITY_ROTATION_MIN, VELOCITY_ROTATION_MAX );
		}
		inline void setVelocityZoom( const float p_value ) noexcept
		{
			_velocityZoom = std::clamp( p_value, VELOCITY_ZOOM_MIN, VELOCITY_ZOOM_MAX );
		}

		inline Mat4f computeViewMatrix() const noexcept { return Util::Math::inverse( _transform.computeMatrix() ); }
		inline Mat4f computeProjectionMatrix() const noexcept { return _camera.computeProjectionMatrix( getPosition() ); }

	  private:
		inline void _updateTarget() noexcept { _camera.target = _transform.getPosition() + _transform.getFront(); }

		Util::Math::Transform _transform;
		Renderer::Camera	  _camera;
		float				  _velocityTranslation = VELOCITY_TRANSLATION_DEFAULT;
		float				  _velocityRotation	  = VELOCITY_ROTATION_DEFAULT;
		float				  _velocityZoom		  = VELOCITY_ZOOM_DEFAULT;
	};
} // namespace VTX::Bench

#endif
