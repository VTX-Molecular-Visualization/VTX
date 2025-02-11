#ifndef __VTX_RENDERER_BENCH_CAMERA__
#define __VTX_RENDERER_BENCH_CAMERA__

#include <util/callback.hpp>
#include <util/constants.hpp>
#include <util/math.hpp>
#include <util/types.hpp>

namespace VTX::Bench
{
	class Camera
	{
	  public:
		Camera() = delete;
		Camera( const size_t p_width, const size_t p_height ) : _width( p_width ), _height( p_height )
		{
			_updateMatrixView();
			_updateMatrixProjection();
		}

		inline const Mat4f &	   getMatrixView() const { return _matrixView; }
		inline const Mat4f &	   getMatrixProjection() const { return _matrixProjection; }
		inline const Mat4f * const getMatrixViewPtr() const { return &_matrixView; }
		inline const Mat4f * const getMatrixProjectionPtr() const { return &_matrixProjection; }
		inline const Vec3f &	   getPosition() const { return _position; }
		inline float			   getNear() const { return _near; }
		inline float			   getFar() const { return _far; }
		inline float			   getFov() const { return _fov; }
		inline bool				   isPerspective() const { return _isPerspective; }
		inline float			   getVelocityTranslation() const { return _velocityTranslation; }
		inline float			   getVelocityRotation() const { return _velocityRotation; }
		inline float			   getVelocityZoom() const { return _velocityZoom; }

		inline void resize( const size_t p_width, const size_t p_height )
		{
			_width	= p_width;
			_height = p_height;
			_updateMatrixProjection();
		}

		inline void setFov( const float p_fov )
		{
			_fov = p_fov;
			_updateMatrixProjection();
		}

		inline void setNear( const float p_near )
		{
			_near = p_near;
			callbackClipInfos( _near, _far );
			_updateMatrixProjection();
		}

		inline void setFar( const float p_far )
		{
			_far = p_far;
			callbackClipInfos( _near, _far );
			_updateMatrixProjection();
		}

		inline void setVelocityTranslation( const float p_velocity ) { _velocityTranslation = p_velocity; }

		inline void setVelocityRotation( const float p_velocity ) { _velocityRotation = p_velocity; }

		inline void setVelocityZoom( const float p_velocity ) { _velocityZoom = p_velocity; }

		inline void translate( const Vec3f & p_delta )
		{
			_position += p_delta.x * _right * _velocityTranslation;
			_position += p_delta.y * _up * _velocityTranslation;
			_position += p_delta.z * _front * _velocityTranslation;

			callbackTranslation( _position );
			_updateMatrixView();
		}

		inline void rotate( const Vec3f & p_delta )
		{
			_rotation = _rotation * Quatf( p_delta * _velocityRotation );

			const Mat3f rotation = Util::Math::castMat3( _rotation );
			_front				 = rotation * _CAMERA_FRONT_DEFAULT;
			_right				 = rotation * _CAMERA_RIGHT_DEFAULT;
			_up					 = rotation * _CAMERA_UP_DEFAULT;
			_updateMatrixView();
		}

		inline void zoom( const float p_delta )
		{
			_fov += p_delta * _velocityZoom;
			_updateMatrixProjection();
		}

		inline static const float NEAR_DEFAULT				   = 1e-1f;
		inline static const float NEAR_MIN					   = 1e-1f;
		inline static const float NEAR_MAX					   = 10.f;
		inline static const float FAR_DEFAULT				   = 1e4f;
		inline static const float FAR_MIN					   = 1e-1f;
		inline static const float FAR_MAX					   = 1e6f;
		inline static const float FOV_DEFAULT				   = 60.f;
		inline static const float FOV_MIN					   = 1e-1f;
		inline static const float FOV_MAX					   = 180.f;
		inline static const float VELOCITY_TRANSLATION_DEFAULT = 100.f;
		inline static const float VELOCITY_TRANSLATION_MIN	   = 1.f;
		inline static const float VELOCITY_TRANSLATION_MAX	   = 1000.f;
		inline static const float VELOCITY_ROTATION_DEFAULT	   = 1.f;
		inline static const float VELOCITY_ROTATION_MIN		   = 1e-1f;
		inline static const float VELOCITY_ROTATION_MAX		   = 10.f;
		inline static const float VELOCITY_ZOOM_DEFAULT		   = 100.f;
		inline static const float VELOCITY_ZOOM_MIN			   = 10.f;
		inline static const float VELOCITY_ZOOM_MAX			   = 1000.f;

		Util::Callback<Mat4f &>		 callbackMatrixView;
		Util::Callback<Mat4f &>		 callbackMatrixProjection;
		Util::Callback<Vec3f &>		 callbackTranslation;
		Util::Callback<float, float> callbackClipInfos;
		Util::Callback<bool>		 callbackPerspective;

	  private:
		const Vec3f _CAMERA_FRONT_DEFAULT = -VEC3F_Z;
		const Vec3f _CAMERA_RIGHT_DEFAULT = VEC3F_X;
		const Vec3f _CAMERA_UP_DEFAULT	  = VEC3F_Y;

		Vec3f _front = _CAMERA_FRONT_DEFAULT;
		Vec3f _right = _CAMERA_RIGHT_DEFAULT;
		Vec3f _up	 = _CAMERA_UP_DEFAULT;

		size_t _width;
		size_t _height;

		float _near = NEAR_DEFAULT;
		float _far	= FAR_DEFAULT;
		float _fov	= FOV_DEFAULT;

		bool  _isPerspective = true;
		Vec3f _position		 = Vec3f( 0.f, 0.f, 100.f );
		Quatf _rotation		 = QUATF_ID;

		Mat4f _matrixView;
		Mat4f _matrixProjection;

		float _velocityTranslation = VELOCITY_TRANSLATION_DEFAULT;
		float _velocityRotation	   = VELOCITY_ROTATION_DEFAULT;
		float _velocityZoom		   = VELOCITY_ZOOM_DEFAULT;

		inline void _updateMatrixView()
		{
			_matrixView = Util::Math::lookAt( _position, _position + _front, _up );
			callbackMatrixView( _matrixView );
		}

		inline void _updateMatrixProjection()
		{
			_matrixProjection = Util::Math::perspective(
				Util::Math::radians( _fov ), float( _width ) / float( _height ), _near, _far
			);
			callbackMatrixProjection( _matrixProjection );
		}
	};
} // namespace VTX::Bench

#endif
