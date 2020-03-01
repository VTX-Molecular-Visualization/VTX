#ifndef __VTX__MATH__
#define __VTX__MATH__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "glm/gtc/matrix_transform.hpp"
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
			static std::mt19937							 gen( 7937 );
			static std::uniform_real_distribution<float> dis( 0.f, 1.f );

			static inline Vec3f min( const Vec3f & a, const Vec3f & b )
			{
				return Vec3f( a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z );
			}

			static inline Vec3f max( const Vec3f & a, const Vec3f & b )
			{
				return Vec3f( a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z );
			}

			static inline float randomInterpolant() { return RANDOM ? dis( rd ) : dis( gen ); }

			static inline Vec3f randomVec3f()
			{
				return Vec3f( randomInterpolant(), randomInterpolant(), randomInterpolant() );
			}

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
		} // namespace Math

	} // namespace Util
} // namespace VTX

#endif
