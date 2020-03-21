#ifndef __VTX__MATH__
#define __VTX__MATH__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/spline.hpp>
#include <glm/gtx/string_cast.hpp>
#include <random>

#undef min
#undef max

namespace VTX
{
	namespace Util
	{
		namespace Math
		{
			static std::random_device					 rd;
			static std::mt19937							 gen( RANDOM ? rd() : 7937 );
			static std::uniform_real_distribution<float> dis( 0.f, 1.f );

			template<typename T>
			static inline T min( const T & a, const T & b )
			{
				return glm::min( a, b );
			}

			template<typename T>
			static inline T max( const T & a, const T & b )
			{
				return glm::max( a, b );
			}

			template<typename T>
			static inline T dot( const T & a, const T & b )
			{
				return glm::dot( a, b );
			}

			template<typename T>
			static inline T cross( const T & a, const T & b )
			{
				return glm::cross( a, b );
			}

			template<typename T>
			static inline auto length( const T & p_value )
			{
				return glm::length( p_value );
			}

			template<typename T>
			static inline T normalize( const T & p_value )
			{
				return glm::normalize( p_value );
			}

			static inline float randomFloat() { return dis( gen ); }

			static inline Vec3f randomVec3f() { return Vec3f( randomFloat(), randomFloat(), randomFloat() ); }

			static inline Quatd eulerToQuaternion( const Vec3f & p_angles ) { return Quatf( p_angles ); }

			static inline Quatd eulerToQuaternion( const double p_pitch, const double p_yaw, const double p_roll )
			{
				// https://www.wikiwand.com/en/Conversion_between_quaternions_and_Euler_angles

				/*9
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

				// Exact same thing?
				Quatd q = eulerToQuaternion( Vec3d( p_pitch, p_yaw, p_roll ) );
				return q;
			}

			static inline Vec3f quaternionToEuler( const Quatd & p_quaternion )
			{
				// https://stackoverflow.com/questions/53033620/how-to-convert-euler-angles-to-quaternions-and-get-the-same-euler-angles-back-fr
				/*
				const Quatf & q = p_quaternion;
				float		  x = q.x;
				float		  y = q.y;
				float		  z = q.z;
				float		  w = q.w;

				float t0 = 2.f * ( w * x + y * z );
				float t1 = 1.f - 2.f * ( x * x + y * y );
				float t2 = 2.f * ( w * y - z * x );
				t2		 = glm::clamp( t2, -1.f, 1.f );
				float t3 = 2.f * ( w * z + x * y );
				float t4 = 1.f - 2.f * ( y * y + z * z );

				return Vec3f( atan2( t0, t1 ), asin( t2 ), atan2( t3, t4 ) );
				*/

				// https://www.wikiwand.com/en/Conversion_between_quaternions_and_Euler_angles

				const Quatd & q = p_quaternion;
				Vec3d		  angles;

				// roll (x-axis rotation)
				double sinr_cosp = 2 * ( q.w * q.x + q.y * q.z );
				double cosr_cosp = 1 - 2 * ( q.x * q.x + q.y * q.y );
				angles.z		 = std::atan2( sinr_cosp, cosr_cosp );

				// pitch (y-axis rotation)
				double sinp = 2 * ( q.w * q.y - q.z * q.x );
				if ( std::abs( sinp ) >= 1 )
					angles.x = std::copysign( PI_2f / 2, sinp ); // use 90 degrees if out of range
				else
					angles.x = std::asin( sinp );

				// yaw (z-axis rotation)
				double siny_cosp = 2 * ( q.w * q.z + q.x * q.y );
				double cosy_cosp = 1 - 2 * ( q.y * q.y + q.z * q.z );
				angles.y		 = std::atan2( siny_cosp, cosy_cosp );

				return angles;
			}

			template<typename T>
			static inline T lerp( const T & p_lhs, const T & p_rhs, const float p_value )
			{
				return glm::lerp( p_lhs, p_rhs, p_value );
			}

			/*
			template<typename T>
			static inline T slerp( const T & p_lhs, const T & p_rhs, const float p_value )
			{
				return glm::slerp( p_lhs, p_rhs, p_value );
			}

			template<typename T>
			static inline T mix( const T & p_lhs, const T & p_rhs, const float p_value )
			{
				return glm::mix( p_lhs, p_rhs, p_value );
			}
			*/

			template<typename T>
			static T catmullRomInterpolation( const T &	  p_p0,
											  const T &	  p_p1,
											  const T &	  p_p2,
											  const T &	  p_p3,
											  const float p_value )
			{
				return glm::catmullRom( p_p0, p_p1, p_p2, p_p3, p_value );
			}

			template<typename T>
			static T cubicInterpolation( const T &	 p_p0,
										 const T &	 p_p1,
										 const T &	 p_p2,
										 const T &	 p_p3,
										 const float p_value )
			{
				return glm::cubic( p_p0, p_p1, p_p2, p_p3, p_value );
			}

			// Morton utils
			static uint leftShift3( uint p_x )
			{
				assert( p_x <= ( 1 << 10 ) );

				if ( p_x == ( 1 << 10 ) ) --p_x;
				p_x = ( p_x | ( p_x << 16 ) ) & 0x30000ff; // x = ---- --98 ---- ---- ---- ---- 7654 3210
				p_x = ( p_x | ( p_x << 8 ) ) & 0x300f00f;  // x = ---- --98 ---- ---- 7654 ---- ---- 3210
				p_x = ( p_x | ( p_x << 4 ) ) & 0x30c30c3;  // x = ---- --98 ---- 76-- --54 ---- 32-- --10
				p_x = ( p_x | ( p_x << 2 ) ) & 0x9249249;  // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0return x;
				return p_x;
			}

			static uint encodeMorton3( const Vec3f & p_v )
			{
				assert( p_v.x >= 0 );
				assert( p_v.y >= 0 );
				assert( p_v.z >= 0 );
				return ( leftShift3( uint( p_v.z ) ) << 2 ) | ( leftShift3( uint( p_v.y ) ) << 1 )
					   | leftShift3( uint( p_v.x ) );
			}

			// p_n (normal) must be normalized
			static Mat3f createOrthonormalBasis( const Vec3f & p_n )
			{
				Vec3f t = fabsf( p_n.x ) > fabsf( p_n.y )
							  ? Vec3f( p_n.z, 0.f, -p_n.x ) / sqrtf( p_n.x * p_n.x + p_n.z * p_n.z )
							  : Vec3f( 0.f, -p_n.z, p_n.y ) / sqrtf( p_n.y * p_n.y + p_n.z * p_n.z );
				Vec3f b = cross( p_n, t );
				return Mat3f( t, b, p_n );
			}
		} // namespace Math

	} // namespace Util
} // namespace VTX

#endif
