#ifndef __VTX_APP_RENDER_RENDERER_RT_RAY__
#define __VTX_APP_RENDER_RENDERER_RT_RAY__

#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::App::Render::Renderer::RT
{
	class Ray
	{
	  public:
		Ray() = default;
		// direction should be normalized
		Ray( const Vec3f & p_origin, const Vec3f & p_direction ) : _origin( p_origin ), _direction( p_direction ) {}
		~Ray()								 = default;
		Ray( const Ray & p_ray )			 = default; // copy constructor
		Ray( Ray && p_ray )					 = default; // move constructor
		Ray & operator=( const Ray & p_ray ) = default; // = copy
		Ray & operator=( Ray && p_ray )		 = default; // = move

		const Vec3f & getOrigin() const { return _origin; }
		const Vec3f & getDirection() const { return _direction; }

		// See:
		// Wachter C., Binder N. (2019)
		// A Fast and Robust Method for Avoiding Self-Intersection.
		// In: Haines E., Akenine-Moller T. (eds) Ray Tracing Gems. Apress, Berkeley, CA
		void offset( const Vec3f p_normal )
		{
			Vec3i of_i = p_normal * intScale();
			Vec3i ip_i( *reinterpret_cast<const int *>( &_origin.x ) + ( ( _origin.x < 0.f ) ? -of_i.x : of_i.x ),
						*reinterpret_cast<const int *>( &_origin.y ) + ( ( _origin.y < 0.f ) ? -of_i.y : of_i.y ),
						*reinterpret_cast<const int *>( &_origin.z ) + ( ( _origin.z < 0.f ) ? -of_i.z : of_i.z ) );

			Vec3f p_i( *reinterpret_cast<const float *>( &ip_i[ 0 ] ),
					   *reinterpret_cast<const float *>( &ip_i[ 1 ] ),
					   *reinterpret_cast<const float *>( &ip_i[ 2 ] ) );
			_origin = Vec3f( fabsf( _origin.x ) < origin() ? _origin.x + floatScale() * p_normal.x : p_i.x,
							 fabsf( _origin.y ) < origin() ? _origin.y + floatScale() * p_normal.y : p_i.y,
							 fabsf( _origin.z ) < origin() ? _origin.z + floatScale() * p_normal.z : p_i.z );
		}

		Vec3f getPointAtT( const float p_t ) const { return _origin + _direction * p_t; }

	  private:
		constexpr float origin() { return 1.f / 32.f; }
		constexpr float floatScale() { return 1.f / 65536.f; }
		constexpr float intScale() { return 256.f; }

	  private:
		Vec3f _origin	 = VEC3F_ZERO;
		Vec3f _direction = VEC3F_Z;
	};
} // namespace VTX::App::Render::Renderer::RT

#endif
