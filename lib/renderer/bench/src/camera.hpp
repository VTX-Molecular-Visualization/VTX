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
		Camera( const size_t p_width, const size_t p_height ) : _width( p_width ), _height( p_height ) {}

		inline const float getNear() const { return _near; }
		inline const float getFar() const { return _far; }
		inline const float getFov() const { return _fov; }
		inline const float getVelocityTranslation() const { return _velocityTranslation; }
		inline const float getVelocityRotation() const { return _velocityRotation; }
		inline const float getVelocityZoom() const { return _velocityZoom; }

		inline void resize( const size_t p_width, const size_t p_height )
		{
			_width	= p_width;
			_height = p_height;
			_onMatrixProjection();
		}

		inline void setFov( const float p_fov )
		{
			_fov = p_fov;
			_onMatrixProjection();
		}

		inline void setNear( const float p_near )
		{
			_near = p_near;
			_onClipInfos();
			_onMatrixProjection();
		}

		inline void setFar( const float p_far )
		{
			_far = p_far;
			_onClipInfos();
			_onMatrixProjection();
		}

		inline void setVelocityTranslation( const float p_velocity ) { _velocityTranslation = p_velocity; }

		inline void setVelocityRotation( const float p_velocity ) { _velocityRotation = p_velocity; }

		inline void setVelocityZoom( const float p_velocity ) { _velocityZoom = p_velocity; }

		inline void translate( const Vec3f & p_delta )
		{
			_position += p_delta.x * _right * _velocityTranslation;
			_position += p_delta.y * _up * _velocityTranslation;
			_position += p_delta.z * _front * _velocityTranslation;
			_onTranslation();
			_onMatrixView();
		}

		inline void rotate( const Vec3f & p_delta )
		{
			_rotation = _rotation * Quatf( p_delta * _velocityRotation );

			const Mat3f rotation = Util::Math::castMat3( _rotation );
			_front				 = rotation * _CAMERA_FRONT_DEFAULT;
			_right				 = rotation * _CAMERA_RIGHT_DEFAULT;
			_up					 = rotation * _CAMERA_UP_DEFAULT;
			_onMatrixView();
		}

		inline void zoom( const float p_delta )
		{
			_fov += p_delta * _velocityZoom;
			_onMatrixProjection();
		}

		// Callbacks.
		inline void addCallbackMatrixView( const Util::Callback<Mat4f>::Func & p_callback )
		{
			_callbackMatrixView += p_callback;
			_onMatrixView();
		}

		inline void addCallbackMatrixProjection( const Util::Callback<Mat4f>::Func & p_callback )
		{
			_callbackMatrixProjection += p_callback;
			_onMatrixProjection();
		}

		inline void addCallbackTranslation( const Util::Callback<Vec3f>::Func & p_callback )
		{
			_callbackTranslation += p_callback;
			_onTranslation();
		}

		inline void addCallbackClipInfos( const Util::Callback<float, float>::Func & p_callback )
		{
			_callbackClipInfos += p_callback;
			_onClipInfos();
		}

		inline void addCallbackPerspective( const Util::Callback<bool>::Func & p_callback )
		{
			_callbackIsPerspective += p_callback;
			_onPerspective();
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
		Vec3f _position		 = Vec3f( 0.f, 0.f, 10.f );
		Quatf _rotation		 = QUATF_ID;

		float _velocityTranslation = VELOCITY_TRANSLATION_DEFAULT;
		float _velocityRotation	   = VELOCITY_ROTATION_DEFAULT;
		float _velocityZoom		   = VELOCITY_ZOOM_DEFAULT;

		Util::Callback<Mat4f>		 _callbackMatrixView;
		Util::Callback<Mat4f>		 _callbackMatrixProjection;
		Util::Callback<Vec3f>		 _callbackTranslation;
		Util::Callback<float, float> _callbackClipInfos;
		Util::Callback<bool>		 _callbackIsPerspective;

		inline void _onMatrixView() { _callbackMatrixView( Util::Math::lookAt( _position, _position + _front, _up ) ); }

		inline void _onMatrixProjection()
		{
			_callbackMatrixProjection(
				Util::Math::perspective( Util::Math::radians( _fov ), float( _width ) / float( _height ), _near, _far )
			);
		}

		inline void _onTranslation() { _callbackTranslation( _position ); }

		inline void _onClipInfos() { _callbackClipInfos( _near, _far ); }

		inline void _onPerspective() { _callbackIsPerspective( _isPerspective ); }
	};
} // namespace VTX::Bench

#endif
