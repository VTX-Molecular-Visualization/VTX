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

		inline void translate( const Vec3f & p_moveInputs )
		{
			_position += p_moveInputs * _FRONT * _translationVelocity;
			_position += p_moveInputs * _RIGHT * _translationVelocity;
			_position += p_moveInputs * _UP * _translationVelocity;
			_updateMatrixView();
		}

		void resize( const size_t p_width, const size_t p_height )
		{
			_width	= p_width;
			_height = p_height;
			_updateMatrixProjection();
		}

		void setCallbackMatrixView( const std::function<void( const Mat4f & )> & p_callback )
		{
			_callbackMatrixView = p_callback;
			_updateMatrixView();
		}

		void setCallbackMatrixProjection( const std::function<void( const Mat4f & )> & p_callback )
		{
			_callbackMatrixProjection = p_callback;
			_updateMatrixProjection();
		}

	  private:
		const Vec3f _FRONT = -VEC3F_Z;
		const Vec3f _RIGHT = VEC3F_X;
		const Vec3f _UP	   = VEC3F_Y;

		size_t _width;
		size_t _height;
		float  _fov		 = 60.f;
		float  _near	 = 0.0001f;
		float  _far		 = 1e4f;
		Vec3f  _position = Vec3f( 0.f, 0.f, 10.f );

		float _translationVelocity = 10.f;

		std::function<void( const Mat4f & )> _callbackMatrixView;
		std::function<void( const Mat4f & )> _callbackMatrixProjection;

		void _updateMatrixView()
		{
			if ( _callbackMatrixView )
			{
				_callbackMatrixView( Util::Math::lookAt( _position, _position + _FRONT, _UP ) );
			}
		}
		void _updateMatrixProjection()
		{
			if ( _callbackMatrixProjection )
			{
				_callbackMatrixProjection( Util::Math::perspective(
					Util::Math::radians( _fov ), float( _width ) / float( _height ), _near, _far ) );
			}
		}
	};
} // namespace VTX::Bench

#endif
