#ifndef __VTX__UTIL_MATH__
#define __VTX__UTIL_MATH__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "exception.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/integer.hpp>
#include <glm/gtx/spline.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>
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
			inline T min( const T & p_lhs, const T & p_rhs )
			{
				return glm::min( p_lhs, p_rhs );
			}

			template<typename T>
			inline T max( const T & p_lhs, const T & p_rhs )
			{
				return glm::max( p_lhs, p_rhs );
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

			template<typename T>
			inline void normalizeSelf( T & p_value )
			{
#ifdef _DEBUG
				T value = glm::normalize( p_value );
				if ( std::isnan( value.x ) ) { throw Exception::MathException( "Normalized value NaN" ); }
				p_value = value;
#else
				p_value = glm::normalize( p_value );
#endif
			}

			template<typename T>
			inline T normalize( const T & p_value )
			{
#ifdef _DEBUG
				T value = glm::normalize( p_value );
				if ( std::isnan( value.x ) ) { throw Exception::MathException( "Normalized value NaN" ); }
				return value;
#else
				return glm::normalize( p_value );
#endif
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
			inline glm::tmat4x4<T> lookAt( const glm::tvec3<T> & p_value,
										   const glm::tvec3<T> & p_target,
										   const glm::tvec3<T> & p_axis )
			{
				return glm::lookAt( p_value, p_target, p_axis );
			}

			template<typename T>
			inline glm::tmat4x4<T> perspective( const T p_fov, const T p_aspect, const T p_near, const T p_far )
			{
				return glm::perspective( p_fov, p_aspect, p_near, p_far );
			}

			template<typename T>
			inline std::string to_string( const T & p_value )
			{
				return glm::to_string( p_value );
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

			inline Quatd eulerToQuaternion( const Vec3f & p_angles ) { return Quatf( p_angles ); }

			inline Quatd eulerToQuaternion( const double p_pitch, const double p_yaw, const double p_roll )
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
				Quatd q = eulerToQuaternion( Vec3d( p_pitch, p_yaw, p_roll ) );
				return q;
			}

			inline Vec3f quaternionToEuler( const Quatd & p_quaternion )
			{
				// https://www.wikiwand.com/en/Conversion_between_quaternions_and_Euler_angles
				const Quatd & q = p_quaternion;
				Vec3d		  angles;

				// Roll (x-axis rotation).
				double sinr_cosp = 2 * ( q.w * q.x + q.y * q.z );
				double cosr_cosp = 1 - 2 * ( q.x * q.x + q.y * q.y );
				angles.z		 = std::atan2( sinr_cosp, cosr_cosp );

				// Pitch (y-axis rotation)
				double sinp = 2 * ( q.w * q.y - q.z * q.x );
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
				double siny_cosp = 2 * ( q.w * q.z + q.x * q.y );
				double cosy_cosp = 1 - 2 * ( q.y * q.y + q.z * q.z );
				angles.y		 = std::atan2( siny_cosp, cosy_cosp );

				return angles;
			}

			template<typename T>
			inline T linearInterpolation( const T & p_lhs, const T & p_rhs, const float p_value )
			{
				return glm::lerp( p_lhs, p_rhs, p_value );
			}

			template<typename T>
			inline T catmullRomInterpolation( const T &	  p_p0,
											  const T &	  p_p1,
											  const T &	  p_p2,
											  const T &	  p_p3,
											  const float p_value )
			{
				return glm::catmullRom( p_p0, p_p1, p_p2, p_p3, p_value );
			}

			template<typename T>
			inline T cubicInterpolation( const T &	 p_p0,
										 const T &	 p_p1,
										 const T &	 p_p2,
										 const T &	 p_p3,
										 const float p_value )
			{
				return glm::cubic( p_p0, p_p1, p_p2, p_p3, p_value );
			}

			// Morton utils
			inline uint leftShift3( uint p_x )
			{
				assert( p_x <= ( 1 << 10 ) );

				if ( p_x == ( 1 << 10 ) ) --p_x;
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

			// p_n (normal) must be normalized
			inline Mat3f createOrthonormalBasis( const Vec3f & p_n )
			{
				Vec3f t = fabsf( p_n.x ) > fabsf( p_n.y )
							  ? Vec3f( p_n.z, 0.f, -p_n.x ) / sqrtf( p_n.x * p_n.x + p_n.z * p_n.z )
							  : Vec3f( 0.f, -p_n.z, p_n.y ) / sqrtf( p_n.y * p_n.y + p_n.z * p_n.z );
				Vec3f b = cross( p_n, t );
				return Mat3f( t, b, p_n );
			}

			template<typename T>
			static float torsionalAngle( const T & p_at0, const T & p_at1, const T & p_at2, const T & p_at3 )
			{
				Vec3f r01 = p_at0 - p_at1;
				Vec3f r32 = p_at3 - p_at2;
				Vec3f r12 = p_at1 - p_at2;

				Vec3f p = cross( r12, r01 );
				Vec3f q = cross( r12, r32 );
				Vec3f r = cross( r12, q );

				float u = dot( q, q );
				float v = dot( r, r );

				float a;
				if ( u <= 0.f || v <= 0.f ) { a = 360.f; }
				else
				{
					float u1 = dot( p, q );
					float v1 = dot( p, r );

					u = u1 / std::sqrt( u );
					v = v1 / std::sqrt( v );

					if ( std::abs( u ) > 0.01f || std::abs( v ) > 0.01f ) { a = degrees( std::atan2( v, u ) ); }
					else
					{
						a = 360.f;
					}
				}
				return a;
			}

			inline Vec3f linearComb( const float   p_scalar0,
									 const Vec3f & p_vector0,
									 const float   p_scalar1,
									 const Vec3f & p_vector1 )
			{
				return p_scalar0 * p_vector0 + p_scalar1 * p_vector1;
			}

		} // namespace Math

	} // namespace Util
} // namespace VTX

#endif
