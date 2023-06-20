#ifndef __VTX_RENDERER_BENCH_CAMERA__
#define __VTX_RENDERER_BENCH_CAMERA__

#include <functional>
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

		inline float getNear() const { return _near; }
		inline float getFar() const { return _far; }
		inline float getFov() const { return _fov; }

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
			_updateClipInfos();
			_updateMatrixProjection();
		}

		inline void setFar( const float p_far )
		{
			_far = p_far;
			_updateClipInfos();
			_updateMatrixProjection();
		}

		inline void translate( const Vec3f & p_moveInputs )
		{
			_position += p_moveInputs * _FRONT * _translationVelocity;
			_position += p_moveInputs * _RIGHT * _translationVelocity;
			_position += p_moveInputs * _UP * _translationVelocity;
			_updateMatrixView();
		}

		// Callbacks.
		inline void setCallbackMatrixView( const std::function<void( const Mat4f & )> & p_callback )
		{
			_callbackMatrixView = p_callback;
			_updateMatrixView();
		}

		inline void setCallbackMatrixProjection( const std::function<void( const Mat4f & )> & p_callback )
		{
			_callbackMatrixProjection = p_callback;
			_updateMatrixProjection();
		}

		inline void setCallbackClipInfos( const std::function<void( const float, const float )> & p_callback )
		{
			_callbackClipInfos = p_callback;
			_updateClipInfos();
		}

	  private:
		const Vec3f _FRONT = -VEC3F_Z;
		const Vec3f _RIGHT = VEC3F_X;
		const Vec3f _UP	   = VEC3F_Y;

		size_t _width;
		size_t _height;

		float _near = 1e-1f;
		float _far	= 1e4f;
		float _fov	= 60.f;

		Vec3f _position = Vec3f( 0.f, 0.f, 10.f );

		float _translationVelocity = 10.f;

		std::function<void( const Mat4f & )>			_callbackMatrixView;
		std::function<void( const Mat4f & )>			_callbackMatrixProjection;
		std::function<void( const float, const float )> _callbackClipInfos;

		inline void _updateMatrixView()
		{
			if ( _callbackMatrixView )
			{
				_callbackMatrixView( Util::Math::lookAt( _position, _position + _FRONT, _UP ) );
			}
		}

		inline void _updateMatrixProjection()
		{
			if ( _callbackMatrixProjection )
			{
				_callbackMatrixProjection( Util::Math::perspective(
					Util::Math::radians( _fov ), float( _width ) / float( _height ), _near, _far ) );
			}
		}

		inline void _updateClipInfos()
		{
			if ( _callbackClipInfos )
			{
				_callbackClipInfos( _near, _far );
			}
		}
	};
} // namespace VTX::Bench

#endif
