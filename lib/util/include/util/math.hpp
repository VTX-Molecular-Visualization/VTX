#ifndef __VTX_UTIL_MATH__
#define __VTX_UTIL_MATH__

#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL

#include "constants.hpp"
#include "types.hpp"
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/integer.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/spline.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <random>

namespace VTX::Util::Math
{
	using ::glm::abs;
	using ::glm::all;
	using ::glm::any;
	using ::glm::clamp;
	using ::glm::conjugate;
	using ::glm::cross;
	using ::glm::degrees;
	using ::glm::distance;
	using ::glm::dot;
	using ::glm::eulerAngles;
	using ::glm::inverse;
	using ::glm::length;
	using ::glm::length2;
	using ::glm::lerp;
	using ::glm::lessThan;
	using ::glm::lookAt;
	using ::glm::mat;
	using ::glm::max;
	using ::glm::min;
	using ::glm::normalize;
	using ::glm::ortho;
	using ::glm::perspective;
	using ::glm::pow2;
	using ::glm::quat;
	using ::glm::quatLookAt;
	using ::glm::radians;
	using ::glm::rotate;
	using ::glm::scale;
	using ::glm::to_string;
	using ::glm::toMat3;
	using ::glm::toMat4;
	using ::glm::translate;
	using ::glm::transpose;
	using ::glm::value_ptr;
	using ::glm::vec;

#ifdef _DEBUG
	constexpr bool RANDOM = false;
#else
	constexpr bool RANDOM = true;
#endif

	inline std::random_device					 rd;
	inline std::mt19937							 gen( RANDOM ? rd() : 7937 );
	inline std::uniform_real_distribution<float> dis( 0.f, 1.f );

	template<typename T>
	inline void normalizeSelf( T & p_value )
	{
		assert( length( p_value ) != 0.f );
		p_value = normalize( p_value );
	}

	template<int L, typename T>
	inline std::vector<T> toStdVector( const vec<L, T> & p_value )
	{
		return std::vector<T>( value_ptr( p_value ), value_ptr( p_value ) + L );
	}

	template<int L, typename T>
	inline std::vector<T> toStdVector( const mat<L, L, T> & p_value )
	{
		return std::vector<T>( value_ptr( p_value ), value_ptr( p_value ) + L * L );
	}

	inline float randomFloat() { return dis( gen ); }

	inline Vec3f randomVec3f() { return Vec3f( randomFloat(), randomFloat(), randomFloat() ); }

	template<int L, typename T>
	inline vec<L, T> directionToEuler( vec<L, T> & p_direction )
	{
		// https://stackoverflow.com/questions/1251828/calculate-rotations-to-look-at-a-3d-point
		vec<L, T> normalized = normalize( p_direction );
		T		  rotx		 = atan2( p_direction.y, p_direction.z );
		T		  roty		 = atan2( p_direction.x * cos( rotx ), p_direction.z );
		T		  rotz		 = atan2( cos( rotx ), sin( rotx ) * sin( roty ) );

		return vec<L, T>( rotx, roty, rotz );
	}

	template<typename T, typename Q>
	inline T easeInOutInterpolation( const T & p_lhs, const T & p_rhs, const Q p_value )
	{
		const Q value = pow2( sin( PI_2f * p_value ) );
		return lerp( p_lhs, p_rhs, value );
	}

	// Morton utils.
	constexpr uint leftShift3( uint p_x ) noexcept
	{
		assert( p_x <= ( 1 << 10 ) );

		if ( p_x == ( 1 << 10 ) )
		{
			--p_x;
		}
		p_x = ( p_x | ( p_x << 16 ) ) & 0x30000ff; // x = ---- --98 ---- ---- ---- ---- 7654 3210
		p_x = ( p_x | ( p_x << 8 ) ) & 0x300f00f;  // x = ---- --98 ---- ---- 7654 ---- ---- 3210
		p_x = ( p_x | ( p_x << 4 ) ) & 0x30c30c3;  // x = ---- --98 ---- 76-- --54 ---- 32-- --10
		p_x = ( p_x | ( p_x << 2 ) ) & 0x9249249;  // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0;
		return p_x;
	}

	constexpr uint encodeMorton3( const Vec3f & p_v ) noexcept
	{
		assert( p_v.x >= 0 );
		assert( p_v.y >= 0 );
		assert( p_v.z >= 0 );
		return ( leftShift3( uint( p_v.z ) ) << 2 ) | ( leftShift3( uint( p_v.y ) ) << 1 )
			   | leftShift3( uint( p_v.x ) );
	}

	// p_n (normal) must be normalized.
	inline Mat3f createOrthonormalBasis( const Vec3f & p_n )
	{
		const Vec3f t = fabsf( p_n.x ) > fabsf( p_n.y )
							? Vec3f( p_n.z, 0.f, -p_n.x ) / sqrtf( p_n.x * p_n.x + p_n.z * p_n.z )
							: Vec3f( 0.f, -p_n.z, p_n.y ) / sqrtf( p_n.y * p_n.y + p_n.z * p_n.z );
		const Vec3f b = cross( p_n, t );
		return Mat3f( t, b, p_n );
	}

	template<typename T>
	inline float _torsionalAngle( const T & p_point0, const T & p_point1, const T & p_point2, const T & p_point3 )
	{
		// This function is a temporary test as the actual implementation seems cubersome to some extend. I leave this
		// non-working example as a token of remembrance to investigate for useless operations on the implementation.
		const Vec3f v01 = p_point1 - p_point0;
		const Vec3f v32 = p_point2 - p_point3;
		const Vec3f v12 = p_point2 - p_point1;

		Vec3f q = cross( v12, v32 );
		Vec3f p = cross( v12, v01 );

		q = normalize( q );
		p = normalize( p );

		return angle( p, q );
	}
	template<typename T>
	inline float torsionalAngle( const T & p_point0, const T & p_point1, const T & p_point2, const T & p_point3 )
	{
		const Vec3f v01 = p_point0 - p_point1;
		const Vec3f v32 = p_point3 - p_point2;
		const Vec3f v12 = p_point1 - p_point2;

		const Vec3f p = cross( v12, v01 );
		const Vec3f q = cross( v12, v32 );
		const Vec3f r = cross( v12, q );

		float u = dot( q, q );
		float v = dot( r, r );

		float angle;
		if ( u <= 0.f || v <= 0.f )
		{
			angle = TWO_PIf;
		}
		else
		{
			const float u1 = dot( p, q );
			const float v1 = dot( p, r );

			u = u1 / sqrtf( u );
			v = v1 / sqrtf( v );

			if ( std::abs( u ) > 0.01f || std::abs( v ) > 0.01f )
			{
				angle = std::atan2( v, u );
			}
			else
			{
				angle = TWO_PIf;
			}
		}
		return angle;
	}

	constexpr Vec3f linearComb(
		const float	  p_scalar0,
		const Vec3f & p_vector0,
		const float	  p_scalar1,
		const Vec3f & p_vector1
	) noexcept
	{
		return p_scalar0 * p_vector0 + p_scalar1 * p_vector1;
	}

	constexpr uint nextPowerOfTwoValue( const uint p_baseNumber ) noexcept
	{
		uint i = 1;
		while ( p_baseNumber > i )
		{
			i <<= 1;
		}
		return i;
	}

	constexpr uint nextPowerOfTwoExponent( const uint p_baseNumber ) noexcept
	{
		uint exponent	= 0;
		uint baseNumber = p_baseNumber;
		while ( baseNumber >>= 1 )
		{
			exponent++;
		}
		return exponent;
	}

	// Uniform hemisphere sampling: z is up.
	inline Vec3f uniformHemisphere( const float p_u, const float p_v )
	{
		const float cosTheta = p_u;
		const float phi		 = TWO_PIf * p_v;
		const float sinTheta = sqrtf( 1.f - cosTheta * cosTheta );
		return Vec3f( cosf( phi ) * sinTheta, sinf( phi ) * sinTheta, cosTheta );
	}

	inline float uniformHemispherePdf() { return INV_2PIf; }

	// Cosine weighted hemisphere samping: z is up.
	inline Vec3f cosineWeightedHemisphere( const float p_u = randomFloat(), const float p_v = randomFloat() )
	{
		const float cosTheta = sqrtf( p_u );
		const float sinTheta = sqrtf( 1.f - p_u );
		const float phi		 = TWO_PIf * p_v;
		return Vec3f( cosf( phi ) * sinTheta, sinf( phi ) * sinTheta, cosTheta );
	}

	inline float cosineWeightedHemispherePdf( const float p_cosTheta ) { return p_cosTheta * INV_PIf; }

	// Compute alignment of p_v to p_a (must be power of two).
	template<typename T>
		requires std::is_unsigned_v<T>
	constexpr T alignUp( const T p_v, const T p_a ) noexcept
	{
		assert( ( p_a & ( p_a - 1 ) ) == 0 );
		return ( p_v + ( p_a - 1 ) ) & ~( p_a - 1 );
	}

	std::vector<Vec3f> inline randomUniVectors( const size_t p_size )
	{
		std::vector<Vec3f> noiseData( p_size );
		std::generate(
			noiseData.begin(),
			noiseData.end(),
			[]
			{
				return Util::Math::normalize(
					Vec3f( Util::Math::randomFloat() * 2.f - 1.f, Util::Math::randomFloat() * 2.f - 1.f, 0.f )
				);
			}
		);

		return noiseData;
	}

	// TODO:  std::fabsf.
	Vec3f orthogonalVector( const Vec3f & normal );

} // namespace VTX::Util::Math

#endif
