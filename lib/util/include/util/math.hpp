#ifndef __VTX_UTIL_MATH__
#define __VTX_UTIL_MATH__

#define GLM_FORCE_INLINE
#define GLM_ENABLE_EXPERIMENTAL

#include "constants.hpp"
#include "types.hpp"
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

#ifdef _DEBUG
	constexpr bool RANDOM = false;
#else
	constexpr bool RANDOM = true;
#endif

	inline std::random_device					 rd;
	inline std::mt19937							 gen( RANDOM ? rd() : 7937 );
	inline std::uniform_real_distribution<float> dis( 0.f, 1.f );

	template<typename T>
	inline T min( const T & p_lhs, const T & p_rhs )
	{
		return glm::min( p_lhs, p_rhs );
	}

	template<typename T>
	inline T max( const T & p_lhs, const T & p_rhs )
	{
		return glm::max( p_lhs, p_rhs );
	}

	template<typename T>
	inline T abs( const T & p_value )
	{
		return glm::abs( p_value );
	}

	template<typename T1>
	inline T1 floor( const T1 & p_value )
	{
		return glm::floor( p_value );
	}

	template<typename T1>
	inline T1 ceil( const T1 & p_value )
	{
		return glm::ceil( p_value );
	}

	template<typename T1, typename T2>
	inline T1 clamp( const T1 & p_value, const T2 & p_min, const T2 & p_max )
	{
		return glm::clamp( p_value, p_min, p_max );
	}

	template<typename T1, typename T2>
	inline T1 translate( const T1 & p_value, const T2 & p_translation )
	{
		return glm::translate( p_value, p_translation );
	}

	template<typename T1, typename T2>
	inline T1 rotate( const T1 & p_value, const T2 & p_rotation, const Vec3f & p_axis )
	{
		return glm::rotate( p_value, p_rotation, p_axis );
	}
	template<typename T>
	inline glm::mat<4, 4, T> getRotation( const T & p_pitch, const T & p_yaw, const T & p_roll )
	{
		const Vec3f eulerRad   = Vec3f( p_pitch, p_yaw, p_roll );
		const Quatf quaternion = glm::quat( eulerRad );
		return glm::mat4x4( quaternion );
	}
	template<typename T>
	inline glm::mat<4, 4, T> getRotation( const glm::qua<T> & p_quaternion )
	{
		return glm::mat4_cast( p_quaternion );
	}

	template<typename T1, typename T2>
	inline T1 scale( const T1 & p_value, const T2 & p_scale )
	{
		return glm::scale( p_value, p_scale );
	}

	inline uint factorial( const uint & p_value ) { return glm::factorial( p_value ); }

	template<int L, typename T>
	inline T distance( const glm::vec<L, T> & p_lhs, const glm::vec<L, T> & p_rhs )
	{
		return glm::distance( p_lhs, p_rhs );
	}

	template<int L, typename T>
	inline T length( const glm::vec<L, T> & p_value )
	{
		return glm::length( p_value );
	}

	template<int L, typename T>
	inline T length2( const glm::vec<L, T> & p_value )
	{
		return glm::length2( p_value );
	}

	template<typename T>
	inline void normalizeSelf( T & p_value )
	{
		assert( glm::length( p_value ) != 0.f );
		p_value = glm::normalize( p_value );
	}

	template<typename T>
	inline T normalize( const T & p_value )
	{
		assert( glm::length( p_value ) != 0.f );
		return glm::normalize( p_value );
	}

	template<int L, typename T>
	inline glm::vec<L, bool> lessThan( const glm::vec<L, T> & p_lhs, const glm::vec<L, T> & p_rhs )
	{
		return glm::lessThan( p_lhs, p_rhs );
	}

	template<int L, typename T>
	inline glm::vec<L, bool> greaterThan( const glm::vec<L, T> & p_lhs, const glm::vec<L, T> & p_rhs )
	{
		return glm::greaterThan( p_lhs, p_rhs );
	}

	template<int L, typename T>
	inline T dot( const glm::vec<L, T> & p_lhs, const glm::vec<L, T> & p_rhs )
	{
		return glm::dot( p_lhs, p_rhs );
	}

	template<typename T>
	inline T cross( const T & p_lhs, const T & p_rhs )
	{
		return glm::cross( p_lhs, p_rhs );
	}

	template<int L, typename T>
	inline T angle( const glm::vec<L, T> & p_lhs, const glm::vec<L, T> & p_rhs )
	{
		return glm::angle( p_lhs, p_rhs );
	}

	template<typename T>
	inline glm::mat<3, 3, T> castMat3( const glm::qua<T> & p_value )
	{
		return glm::mat3_cast( p_value );
	}

	template<typename T>
	inline T degrees( const T & p_value )
	{
		return glm::degrees( p_value );
	}

	template<typename T>
	inline T radians( const T & p_value )
	{
		return glm::radians( p_value );
	}

	template<typename T>
	inline glm::tmat4x4<T> lookAt(
		const glm::tvec3<T> & p_value,
		const glm::tvec3<T> & p_target,
		const glm::tvec3<T> & p_axis
	)
	{
		return glm::lookAt( p_value, p_target, p_axis );
	}

	template<typename T>
	inline glm::tmat4x4<T> perspective( const T & p_fov, const T & p_aspect, const T & p_near, const T & p_far )
	{
		return glm::perspective( p_fov, p_aspect, p_near, p_far );
	}

	template<typename T>
	inline glm::tmat4x4<T> ortho(
		const T & p_left,
		const T & p_right,
		const T & p_bottom,
		const T & p_top,
		const T & p_near,
		const T & p_far
	)
	{
		return glm::ortho( p_left, p_right, p_bottom, p_top, p_near, p_far );
	}

	template<typename T>
	inline glm::tmat4x4<T> transpose( const glm::tmat4x4<T> & mat )
	{
		return glm::transpose( mat );
	}

	template<typename T>
	inline glm::tmat4x4<T> inverse( const glm::tmat4x4<T> & mat )
	{
		return glm::inverse( mat );
	}

	template<typename T>
	inline std::string to_string( const T & p_value )
	{
		return glm::to_string( p_value );
	}

	template<typename T>
	inline std::string to_string_fmt( const T & p_value )
	{
		std::string glmString = glm::to_string( p_value );
		for ( int i = 0; i < glmString.size(); i++ )
		{
			const char & currentChar = glmString[ i ];

			if ( currentChar == '{' || currentChar == '}' )
			{
				glmString.insert( i, 1, currentChar );
				i++;
			}
		}

		return glmString;
	}

	template<int L, typename T>
	inline T const * value_ptr( const glm::vec<L, T> & p_value )
	{
		return glm::value_ptr( p_value );
	}

	template<int L, typename T>
	inline T const * value_ptr( const glm::mat<L, L, T> & p_value )
	{
		return glm::value_ptr( p_value );
	}

	template<int L, typename T>
	inline std::vector<T> toStdVector( const glm::vec<L, T> & p_value )
	{
		return std::vector<T>( glm::value_ptr( p_value ), glm::value_ptr( p_value ) + L );
	}

	template<int L, typename T>
	inline std::vector<T> toStdVector( const glm::mat<L, L, T> & p_value )
	{
		return std::vector<T>( glm::value_ptr( p_value ), glm::value_ptr( p_value ) + L * L );
	}

	template<typename T>
	inline T reflect( const T & p_vi, const T & p_vn )
	{
		return glm::reflect( p_vi, p_vn );
	}

	template<int L, typename T>
	inline glm::vec<L, T> refract( const glm::vec<L, T> & p_vi, const glm::vec<L, T> & p_vn, const T & p_eta )
	{
		return glm::refract( p_vi, p_vn, p_eta );
	}

	template<typename T>
	inline T pow( const T & p_value, const T & p_exp )
	{
		return glm::pow( p_value, p_exp );
	}

	template<typename T>
	inline T faceForward( const T & p_vec, const T & p_view )
	{
		return glm::faceforward( p_vec, p_view, p_vec );
	}

	inline float randomFloat() { return dis( gen ); }

	inline Vec3f randomVec3f() { return Vec3f( randomFloat(), randomFloat(), randomFloat() ); }

	inline Quatf toQuat( const Mat4f & p_matrix ) { return glm::toQuat( p_matrix ); }

	inline Quatf eulerToQuaternion( const Vec3f & p_angles ) { return Quatf( p_angles ); }

	inline Quatf eulerToQuaternion( const float & p_pitch, const float & p_yaw, const float & p_roll )
	{
		// https://www.wikiwand.com/en/Conversion_between_quaternions_and_Euler_angles
		/*
		float cy = cos( p_yaw * 0.5f );
		float sy = sin( p_yaw * 0.5f );
		float cp = cos( p_pitch * 0.5f );
		float sp = sin( p_pitch * 0.5f );
		float cr = cos( p_roll * 0.5f );
		float sr = sin( p_roll * 0.5f );

		Quatf q;
		q.w = cy * cp * cr + sy * sp * sr;
		q.x = cy * cp * sr - sy * sp * cr;
		q.y = sy * cp * sr + cy * sp * cr;
		q.z = sy * cp * cr - cy * sp * sr;

		return q;
		*/
		// Same thing did by glm.
		return eulerToQuaternion( Vec3f( p_pitch, p_yaw, p_roll ) );
	}

	inline Vec3f quaternionToEuler( const Quatf & p_quaternion )
	{
		// https://www.wikiwand.com/en/Conversion_between_quaternions_and_Euler_angles
		const Quatf & q = p_quaternion;
		Vec3f		  angles;

		// Roll (x-axis rotation).
		const float sinr_cosp = 2.f * ( q.w * q.x + q.y * q.z );
		const float cosr_cosp = 1.f - 2.f * ( q.x * q.x + q.y * q.y );
		angles.z			  = std::atan2( sinr_cosp, cosr_cosp );

		// Pitch (y-axis rotation).
		const float sinp = 2.f * ( q.w * q.y - q.z * q.x );
		if ( std::abs( sinp ) >= 1 )
		{
			// Use 90 degrees if out of range.
			angles.x = std::copysign( PI_2f / 2, sinp );
		}
		else
		{
			angles.x = std::asin( sinp );
		}

		// Yaw (z-axis rotation).
		const float siny_cosp = 2.f * ( q.w * q.z + q.x * q.y );
		const float cosy_cosp = 1.f - 2.f * ( q.y * q.y + q.z * q.z );
		angles.y			  = std::atan2( siny_cosp, cosy_cosp );

		return angles;
	}

	inline Vec3f rotationMatrixToEuler( const Mat4f & p_matrix )
	{
		const Quatf quat = glm::toQuat( p_matrix );
		return degrees( glm::eulerAngles( quat ) );
	}
	template<int L, typename T>
	inline glm::vec<L, T> directionToEuler( glm::vec<L, T> & p_direction )
	{
		// https://stackoverflow.com/questions/1251828/calculate-rotations-to-look-at-a-3d-point
		glm::vec<L, T> normalized = normalize( p_direction );
		T			   rotx		  = atan2( p_direction.y, p_direction.z );
		T			   roty		  = atan2( p_direction.x * cos( rotx ), p_direction.z );
		T			   rotz		  = atan2( cos( rotx ), sin( rotx ) * sin( roty ) );

		return glm::vec<L, T>( rotx, roty, rotz );
	}

	template<typename T, typename Q>
	inline T linearInterpolation( const T & p_lhs, const T & p_rhs, const Q p_value )
	{
		return glm::lerp( p_lhs, p_rhs, p_value );
	}

	template<typename T, typename Q>
	inline T catmullRomInterpolation( const T & p_p0, const T & p_p1, const T & p_p2, const T & p_p3, const Q p_value )
	{
		return glm::catmullRom( p_p0, p_p1, p_p2, p_p3, p_value );
	}

	template<typename T, typename Q>
	inline T cubicInterpolation( const T & p_p0, const T & p_p1, const T & p_p2, const T & p_p3, const Q p_value )
	{
		return glm::cubic( p_p0, p_p1, p_p2, p_p3, p_value );
	}

	template<typename T, typename Q>
	inline T easeInOutInterpolation( const T & p_lhs, const T & p_rhs, const Q p_value )
	{
		const Q value = glm::pow2( glm::sin( PI_2f * p_value ) );
		return glm::lerp( p_lhs, p_rhs, value );
	}

	// Morton utils.
	inline uint leftShift3( uint p_x )
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

	inline uint encodeMorton3( const Vec3f & p_v )
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

	inline Vec3f linearComb(
		const float	  p_scalar0,
		const Vec3f & p_vector0,
		const float	  p_scalar1,
		const Vec3f & p_vector1
	)
	{
		return p_scalar0 * p_vector0 + p_scalar1 * p_vector1;
	}

	inline uint nextPowerOfTwoValue( const uint p_baseNumber )
	{
		uint i = 1;
		while ( p_baseNumber > i )
		{
			i <<= 1;
		}
		return i;
	}

	inline uint nextPowerOfTwoExponent( const uint p_baseNumber )
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

	// TODO:  std::fabsf.
	Vec3f orthogonalVector( const Vec3f & normal );

} // namespace VTX::Util::Math

#endif
