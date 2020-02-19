#ifndef __VTX__MATH__
#define __VTX__MATH__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/compatibility.hpp>
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
			static std::mt19937							 gen( 79 );
			static std::uniform_real_distribution<float> dis( 0.f, 1.f );

			static inline Vec3f min( const Vec3f & a, const Vec3f & b )
			{
				return Vec3f( a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z );
			}

			static inline Vec3f max( const Vec3f & a, const Vec3f & b )
			{
				return Vec3f( a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z );
			}

			static inline Vec3f randomVec3f() { return Vec3f( dis( gen ), dis( gen ), dis( gen ) ); }

			static inline Quatf eulerToQuaternion( const Vec3f & p_angles ) { return Quatf( p_angles ); }

			static inline Quatf eulerToQuaternion( const float p_pitch, const float p_yaw, const float p_roll )
			{
				// https://stackoverflow.com/questions/53033620/how-to-convert-euler-angles-to-quaternions-and-get-the-same-euler-angles-back-fr

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

				// Exact same thing?
				Quatf q = eulerToQuaternion( Vec3f( p_pitch, p_yaw, p_roll ) );
				//q /= (float)q.length();
				return q;
			}

			static inline Vec3f quaternionToEuler( const Quatf & p_quaternion )
			{
				// https://stackoverflow.com/questions/53033620/how-to-convert-euler-angles-to-quaternions-and-get-the-same-euler-angles-back-fr

				Quatf q = p_quaternion;
				//q /= (float)q.length();

				float x = q.x;
				float y = q.y;
				float z = q.z;
				float w = q.w;

				float t0 = 2.f * ( w * x + y * z );
				float t1 = 1.f - 2.f * ( x * x + y * y );
				float t2 = 2.f * ( w * y - z * x );
				t2		 = glm::clamp( t2, -1.f, 1.f );
				float t3 = 2.f * ( w * z + x * y );
				float t4 = 1.f - 2.f * ( y * y + z * z );

				return Vec3f( atan2( t0, t1 ), asin( t2 ), atan2( t3, t4 ) );
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
			static inline T catmullRomInterpolation( const T & p_lhs, const T & p_rhs, const float p_value )
			{
				T result = T();
				return result;
			}
		} // namespace Math

	} // namespace Util
} // namespace VTX

#endif
