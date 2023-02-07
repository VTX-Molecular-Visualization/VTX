#ifndef __VTX__UTIL_MATH__
#define __VTX__UTIL_MATH__

#define GLM_FORCE_INLINE

#include <util/types.hpp>
#include <util/constants.hpp>
#include "glm/gtc/type_ptr.hpp"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/integer.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/spline.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <math/matrix.hpp>
#include <nlohmann/json.hpp>
#include <random>

namespace VTX
{
	namespace Util
	{
		namespace Math
		{
#ifdef _DEBUG
			constexpr bool RANDOM = false;
#else
			constexpr bool RANDOM = true;
#endif

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
			inline glm::tmat4x4<T> lookAt( const glm::tvec3<T> & p_value,
										   const glm::tvec3<T> & p_target,
										   const glm::tvec3<T> & p_axis )
			{
				return glm::lookAt( p_value, p_target, p_axis );
			}

			template<typename T>
			inline glm::tmat4x4<T> perspective( const T & p_fov, const T & p_aspect, const T & p_near, const T & p_far )
			{
				return glm::perspective( p_fov, p_aspect, p_near, p_far );
			}

			template<typename T>
			inline glm::tmat4x4<T> ortho( const T & p_left,
										  const T & p_right,
										  const T & p_bottom,
										  const T & p_top,
										  const T & p_near,
										  const T & p_far )
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
			inline T catmullRomInterpolation( const T & p_p0,
											  const T & p_p1,
											  const T & p_p2,
											  const T & p_p3,
											  const Q	p_value )
			{
				return glm::catmullRom( p_p0, p_p1, p_p2, p_p3, p_value );
			}

			template<typename T, typename Q>
			inline T cubicInterpolation( const T & p_p0,
										 const T & p_p1,
										 const T & p_p2,
										 const T & p_p3,
										 const Q   p_value )
			{
				return glm::cubic( p_p0, p_p1, p_p2, p_p3, p_value );
			}

			template<typename T, typename Q>
			inline T easeInOutInterpolation( const T & p_lhs, const T & p_rhs, const Q p_value )
			{
				const Q value = glm::pow2( glm::sin( PI_2f * p_value ) );
				return glm::lerp( p_lhs, p_rhs, value );
			}

			// Morton utils
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

			// p_n (normal) must be normalized
			inline Mat3f createOrthonormalBasis( const Vec3f & p_n )
			{
				const Vec3f t = fabsf( p_n.x ) > fabsf( p_n.y )
									? Vec3f( p_n.z, 0.f, -p_n.x ) / sqrtf( p_n.x * p_n.x + p_n.z * p_n.z )
									: Vec3f( 0.f, -p_n.z, p_n.y ) / sqrtf( p_n.y * p_n.y + p_n.z * p_n.z );
				const Vec3f b = cross( p_n, t );
				return Mat3f( t, b, p_n );
			}

			template<typename T>
			static float torsionalAngle( const T & p_point0,
										 const T & p_point1,
										 const T & p_point2,
										 const T & p_point3 )
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

			inline Vec3f linearComb( const float   p_scalar0,
									 const Vec3f & p_vector0,
									 const float   p_scalar1,
									 const Vec3f & p_vector1 )
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

			inline Vec3f orthogonalVector( const Vec3f & normal )
			{
				// Solve a*x + b*y + c*z = 0
				const float a = normal.x;
				const float b = normal.y;
				const float c = normal.z;
				float		x = 0.f, y = 0.f, z = 0.f;
				if ( std::abs( a ) > VTX::EPSILON && std::abs( b ) > VTX::EPSILON )
				{
					x = -glm::sign( a ) * b;
					y = glm::sign( a ) * a;
					z = 0.f;
				}
				else if ( std::abs( b ) > VTX::EPSILON && std::abs( c ) > VTX::EPSILON )
				{
					x = 0.f;
					y = -glm::sign( b ) * c;
					z = glm::sign( b ) * b;
				}
				else if ( std::abs( c ) > VTX::EPSILON && std::abs( a ) > VTX::EPSILON )
				{
					x = glm::sign( c ) * c;
					y = 0.f;
					z = -glm::sign( c ) * a;
				}
				else if ( std::abs( a ) > VTX::EPSILON )
				{
					x = 0.f;
					y = 1.f;
					z = 0.f;
				}
				else if ( std::abs( b ) > VTX::EPSILON )
				{
					x = 0.f;
					y = 0.f;
					z = 1.f;
				}
				else if ( std::abs( c ) > VTX::EPSILON )
				{
					x = 1.f;
					y = 0.f;
					z = 0.f;
				}

				// Vec3f v1 = glm::normalize( Vec3f( x, y, z ) );
				// Vec3f v2 = glm::normalize( Vec3f( b * z - c * y, c * x - a * z, a * y - b * x ) );
				return glm::normalize( Vec3f( x, y, z ) );
			}

			template<typename T>
			inline VTX::Math::Matrix<T> transpose( const VTX::Math::Matrix<T> & p_mat )
			{
				const size_t rowCount	 = p_mat.getRowCount();
				const size_t columnCount = p_mat.getColumnCount();

				VTX::Math::Matrix<T> res( columnCount, rowCount );

				for ( int iRow = 0; iRow < rowCount; iRow++ )
					for ( int iCol = 0; iCol < columnCount; iCol++ )
						res.set( iCol, iRow, p_mat.get( iRow, iCol ) );

				return res;
			}

			template<typename T>
			inline VTX::Math::Matrix<T> matMult( const VTX::Math::Matrix<T> & p_lhs,
												 const VTX::Math::Matrix<T> & p_rhs )
			{
				if ( p_lhs.getColumnCount() != p_rhs.getRowCount() )
					return VTX::Math::Matrix<T>();

				const int M = int( p_lhs.getRowCount() );
				const int N = int( p_lhs.getColumnCount() );
				const int K = int( p_rhs.getColumnCount() );

				VTX::Math::Matrix<T> res( M, K );

				for ( int i = 0; i < M; i++ )
				{
					for ( int j = 0; j < K; j++ )
					{
						T sum = 0;

						for ( int k = 0; k < N; k++ )
							sum += p_lhs.get( i, k ) * p_rhs.get( k, j );

						res.set( i, j, sum );
					}
				}

				return res;
			}

			inline Mat3f toMat3f( const VTX::Math::Matrix<float> & p_mat )
			{
				assert( p_mat.getRowCount() == 3 && p_mat.getColumnCount() == 3 );

				Mat3f res = Mat3f();

				for ( int i = 0; i < 3; i++ )
					for ( int j = 0; j < 3; j++ )
						res[ i ][ j ] = p_mat.get( i, j );

				return res;
			}

			// Taken from https://math.nist.gov/tnt/download.html (Eigen)
			template<typename T>
			inline void getSVDThin( const VTX::Math::Matrix<T> & p_mat,
									VTX::Math::Matrix<T> &		 p_U,
									VTX::Math::Matrix<T> &		 p_V )
			{
				const size_t rowCount = p_mat.getRowCount();
				const size_t colCount = p_mat.getColumnCount();
				const size_t diagSize = rowCount < colCount ? rowCount : colCount;

				VTX::Math::Matrix<T> workingMatrix = VTX::Math::Matrix<T>( p_mat );
				T					 maxCoeff	   = 0;

				for ( int i = 0; i < p_mat.getRowCount(); i++ )
				{
					for ( int j = 0; j < p_mat.getColumnCount(); j++ )
					{
						maxCoeff = std::max( maxCoeff, p_mat.get( i, j ) );
					}
				}

				if ( maxCoeff == 0 )
					maxCoeff = 1;

				for ( int i = 0; i < p_mat.getRowCount(); i++ )
				{
					for ( int j = 0; j < p_mat.getColumnCount(); j++ )
					{
						workingMatrix.set( i, j, p_mat.get( i, j ) / maxCoeff );
					}
				}

				p_U = VTX::Math::Matrix<T>::getidentity( diagSize );
				p_V = VTX::Math::Matrix<T>::getidentity( diagSize );

				// const float maxDiagEntry = m_workMatrix.cwiseAbs().diagonal().maxCoeff();

				// bool finished = false;
				// while ( !finished )
				//{
				//	finished = true;

				//	// do a sweep: for all index pairs (p,q), perform SVD of the corresponding 2x2 sub-matrix
				//	for ( size_t p = 1; p < diagSize; ++p )
				//	{
				//		for ( size_t q = 0; q < p; ++q )
				//		{
				//			// if this 2x2 sub-matrix is not diagonal already...
				//			// notice that this comparison will evaluate to false if any NaN is involved, ensuring that
				//			// NaN's don't keep us iterating forever. Similarly, small denormal numbers are considered
				//			// zero.
				//			float threshold = EPSILON;
				//			if ( std::abs( m_workMatrix.coeff( p, q ) ) > EPSILON
				//				 || std::abs( m_workMatrix.coeff( q, p ) ) > EPSILON )
				//			{
				//				finished = false;
				//				// perform SVD decomposition of 2x2 sub-matrix corresponding to indices p,q to make it
				//				// diagonal the complex to real operation returns true if the updated 2x2 block is not
				//				// already diagonal
				//				if ( !isDiagonal( m_workMatrix, p, q, maxDiagEntry ) )
				//				{
				//					JacobiRotation<RealScalar> j_left, j_right;
				//					internal::real_2x2_jacobi_svd( m_workMatrix, p, q, &j_left, &j_right );

				//					// accumulate resulting Jacobi rotations
				//					m_workMatrix.applyOnTheLeft( p, q, j_left );
				//					if ( computeU() )
				//						m_matrixU.applyOnTheRight( p, q, j_left.transpose() );

				//					m_workMatrix.applyOnTheRight( p, q, j_right );
				//					if ( computeV() )
				//						m_matrixV.applyOnTheRight( p, q, j_right );

				//					// keep track of the largest diagonal coefficient
				//					maxDiagEntry = numext::maxi<RealScalar>(
				//						maxDiagEntry,
				//						numext::maxi<RealScalar>( abs( m_workMatrix.coeff( p, p ) ),
				//												  abs( m_workMatrix.coeff( q, q ) ) ) );
				//				}
				//			}
				//		}
				//	}
				//}
			}

			// template<typename T>
			// static void real_2x2_jacobi_svd( const VTX::Math::Matrix<T> & matrix,
			//								 const int					  p,
			//								 const int					  q,
			//								 VTX::Math::Matrix<T> &		  j_left,
			//								 VTX::Math::Matrix<T> &		  j_right )
			//{
			//	VTX::Math::Matrix<T> m = VTX::Math::Matrix<T>( 2, 2 );
			//	m.set( 0, 0, matrix.coeff( p, p ) );
			//	m.set( 0, 1, matrix.coeff( p, q ) );
			//	m.set( 1, 0, matrix.coeff( q, p ) );
			//	m.set( 1, 1, matrix.coeff( q, q ) );
			//	Matrix<RealScalar, 2, 2> m;
			//	m << numext::real( matrix.coeff( p, p ) ), numext::real( matrix.coeff( p, q ) ),
			//		numext::real( matrix.coeff( q, p ) ), numext::real( matrix.coeff( q, q ) );
			//	JacobiRotation<RealScalar> rot1;
			//	RealScalar				   t = m.coeff( 0, 0 ) + m.coeff( 1, 1 );
			//	RealScalar				   d = m.coeff( 1, 0 ) - m.coeff( 0, 1 );

			//	if ( std::abs( d ) < (std::numeric_limits<RealScalar>::min)() )
			//	{
			//		rot1.s() = RealScalar( 0 );
			//		rot1.c() = RealScalar( 1 );
			//	}
			//	else
			//	{
			//		// If d!=0, then t/d cannot overflow because the magnitude of the
			//		// entries forming d are not too small compared to the ones forming t.
			//		RealScalar u   = t / d;
			//		RealScalar tmp = std::sqrt( RealScalar( 1 ) + numext::abs2( u ) );
			//		rot1.s()	   = RealScalar( 1 ) / tmp;
			//		rot1.c()	   = u / tmp;
			//	}
			//	m.applyOnTheLeft( 0, 1, rot1 );
			//	j_right->makeJacobi( m, 0, 1 );
			//	*j_left = rot1 * j_right->transpose();
			//}
			// bool JacobiRotation<Scalar>::makeJacobi( const RealScalar & x, const Scalar & y, const RealScalar & z )
			//{
			//	using std::abs;
			//	using std::sqrt;

			//	RealScalar deno = RealScalar( 2 ) * abs( y );
			//	if ( deno < (std::numeric_limits<RealScalar>::min)() )
			//	{
			//		m_c = Scalar( 1 );
			//		m_s = Scalar( 0 );
			//		return false;
			//	}
			//	else
			//	{
			//		RealScalar tau = ( x - z ) / deno;
			//		RealScalar w   = sqrt( numext::abs2( tau ) + RealScalar( 1 ) );
			//		RealScalar t;
			//		if ( tau > RealScalar( 0 ) )
			//		{
			//			t = RealScalar( 1 ) / ( tau + w );
			//		}
			//		else
			//		{
			//			t = RealScalar( 1 ) / ( tau - w );
			//		}
			//		RealScalar sign_t = t > RealScalar( 0 ) ? RealScalar( 1 ) : RealScalar( -1 );
			//		RealScalar n	  = RealScalar( 1 ) / sqrt( numext::abs2( t ) + RealScalar( 1 ) );
			//		m_s				  = -sign_t * ( numext::conj( y ) / abs( y ) ) * abs( t ) * n;
			//		m_c				  = n;
			//		return true;
			//	}
			//}

			// Taken from https://math.nist.gov/tnt/download.html (JAMA/C++ Linear Algebra Package)
			template<typename T>
			inline void getSVD( const VTX::Math::Matrix<T> & p_mat,
								VTX::Math::Matrix<T> &		 p_U,
								std::vector<T> &			 p_sigma,
								VTX::Math::Matrix<T> &		 p_V )
			{
				const int m = int( p_mat.getRowCount() );
				const int n = int( p_mat.getColumnCount() );

				const int nu = min( m, n );

				p_sigma = std::vector<T>( min( m + 1, n ) );
				p_U		= VTX::Math::Matrix<T>( m, nu, 0 );
				p_V		= VTX::Math::Matrix<T>( n, n );

				std::vector<T>		 e( n );
				std::vector<T>		 work( m );
				VTX::Math::Matrix<T> A( p_mat );
				const bool			 wantu = true; /* boolean */
				const bool			 wantv = true; /* boolean */
				int					 i = 0, j = 0, k = 0;

				// Reduce A to bidiagonal form, storing the diagonal elements
				// in s and the super-diagonal elements in e.

				const int nct = min( m - 1, n );
				const int nrt = max( 0, min( n - 2, m ) );
				for ( k = 0; k < max( nct, nrt ); k++ )
				{
					if ( k < nct )
					{
						// Compute the transformation for the k-th column and
						// place the k-th diagonal in s[k].
						// Compute 2-norm of k-th column without under/overflow.
						p_sigma[ k ] = 0;
						for ( i = k; i < m; i++ )
						{
							p_sigma[ k ] = hypot( p_sigma[ k ], A.get( i, k ) );
						}
						if ( p_sigma[ k ] != 0.0 )
						{
							if ( A.get( k, k ) < 0.0 )
							{
								p_sigma[ k ] = -p_sigma[ k ];
							}
							for ( i = k; i < m; i++ )
							{
								A.set( i, k, A.get( i, k ) / p_sigma[ k ] );
							}
							A.add( k, k, 1.0 );
						}
						p_sigma[ k ] = -p_sigma[ k ];
					}
					for ( j = k + 1; j < n; j++ )
					{
						if ( ( k < nct ) && ( p_sigma[ k ] != 0.0 ) )
						{
							// Apply the transformation.

							double t = 0;
							for ( i = k; i < m; i++ )
							{
								t += A.get( i, k ) * A.get( i, j );
							}
							t = -t / A.get( k, k );
							for ( i = k; i < m; i++ )
							{
								A.add( i, j, t * A.get( i, k ) );
							}
						}

						// Place the k-th row of A into e for the
						// subsequent calculation of the row transformation.

						e[ j ] = A.get( k, j );
					}
					if ( wantu && ( k < nct ) )
					{
						// Place the transformation in U for subsequent back
						// multiplication.

						for ( i = k; i < m; i++ )
						{
							p_U.set( i, k, A.get( i, k ) );
						}
					}
					if ( k < nrt )
					{
						// Compute the k-th row transformation and place the
						// k-th super-diagonal in e[k].
						// Compute 2-norm without under/overflow.
						e[ k ] = 0;
						for ( i = k + 1; i < n; i++ )
						{
							e[ k ] = hypot( e[ k ], e[ i ] );
						}
						if ( e[ k ] != 0.0 )
						{
							if ( e[ k + 1 ] < 0.0 )
							{
								e[ k ] = -e[ k ];
							}
							for ( i = k + 1; i < n; i++ )
							{
								e[ i ] /= e[ k ];
							}
							e[ k + 1 ] += 1.0;
						}
						e[ k ] = -e[ k ];
						if ( ( k + 1 < m ) & ( e[ k ] != 0.0 ) )
						{
							// Apply the transformation.

							for ( i = k + 1; i < m; i++ )
							{
								work[ i ] = 0.0;
							}
							for ( j = k + 1; j < n; j++ )
							{
								for ( i = k + 1; i < m; i++ )
								{
									work[ i ] += e[ j ] * A.get( i, j );
								}
							}
							for ( j = k + 1; j < n; j++ )
							{
								double t = -e[ j ] / e[ k + 1 ];
								for ( i = k + 1; i < m; i++ )
								{
									A.add( i, j, t * work[ i ] );
								}
							}
						}
						if ( wantv )
						{
							// Place the transformation in V for subsequent
							// back multiplication.

							for ( i = k + 1; i < n; i++ )
							{
								p_V.set( i, k, e[ i ] );
							}
						}
					}
				}

				// Set up the final bidiagonal matrix or order p.

				int p = min( n, m + 1 );
				if ( nct < n )
				{
					p_sigma[ nct ] = A.get( nct, nct );
				}
				if ( m < p )
				{
					p_sigma[ p - 1 ] = 0.0;
				}
				if ( nrt + 1 < p )
				{
					e[ nrt ] = A.get( nrt, p - 1 );
				}
				e[ p - 1 ] = 0.0;

				// If required, generate U.

				if ( wantu )
				{
					for ( j = nct; j < nu; j++ )
					{
						for ( i = 0; i < m; i++ )
						{
							p_U.set( i, j, 0.0 );
						}
						p_U.set( j, j, 1.0 );
					}
					for ( k = nct - 1; k >= 0; k-- )
					{
						if ( p_sigma[ k ] != 0.0 )
						{
							for ( j = k + 1; j < nu; j++ )
							{
								double t = 0;
								for ( i = k; i < m; i++ )
								{
									t += p_U.get( i, k ) * p_U.get( i, j );
								}
								t = -t / p_U.get( k, k );
								for ( i = k; i < m; i++ )
								{
									p_U.add( i, j, t * p_U.get( i, k ) );
								}
							}
							for ( i = k; i < m; i++ )
							{
								p_U.set( i, k, -p_U.get( i, k ) );
							}
							p_U.set( k, k, 1.0 + p_U.get( k, k ) );
							for ( i = 0; i < k - 1; i++ )
							{
								p_U.set( i, k, 0.0 );
							}
						}
						else
						{
							for ( i = 0; i < m; i++ )
							{
								p_U.set( i, k, 0.0 );
							}
							p_U.set( k, k, 1.0 );
						}
					}
				}

				// If required, generate V.

				if ( wantv )
				{
					for ( k = n - 1; k >= 0; k-- )
					{
						if ( ( k < nrt ) & ( e[ k ] != 0.0 ) )
						{
							for ( j = k + 1; j < nu; j++ )
							{
								double t = 0;
								for ( i = k + 1; i < n; i++ )
								{
									t += p_V.get( i, k ) * p_V.get( i, j );
								}
								t = -t / p_V.get( k + 1, k );
								for ( i = k + 1; i < n; i++ )
								{
									p_V.add( i, j, t * p_V.get( i, k ) );
								}
							}
						}
						for ( i = 0; i < n; i++ )
						{
							p_V.set( i, k, 0.0 );
						}
						p_V.set( k, k, 1.0 );
					}
				}

				// Main iteration loop for the singular values.

				int	   pp	= p - 1;
				int	   iter = 0;
				double eps	= pow( 2.0, -52.0 );
				while ( p > 0 )
				{
					int k	 = 0;
					int kase = 0;

					// Here is where a test for too many iterations would go.

					// This section of the program inspects for
					// negligible elements in the s and e arrays.  On
					// completion the variables kase and k are set as follows.

					// kase = 1     if s(p) and e[k-1] are negligible and k<p
					// kase = 2     if s(k) is negligible and k<p
					// kase = 3     if e[k-1] is negligible, k<p, and
					//              s(k), ..., s(p) are not negligible (qr step).
					// kase = 4     if e(p-1) is negligible (convergence).

					for ( k = p - 2; k >= -1; k-- )
					{
						if ( k == -1 )
						{
							break;
						}
						if ( abs( e[ k ] ) <= eps * ( abs( p_sigma[ k ] ) + abs( p_sigma[ k + 1 ] ) ) )
						{
							e[ k ] = 0.0;
							break;
						}
					}
					if ( k == p - 2 )
					{
						kase = 4;
					}
					else
					{
						int ks;
						for ( ks = p - 1; ks >= k; ks-- )
						{
							if ( ks == k )
							{
								break;
							}
							double t = ( ks != p ? abs( e[ ks ] ) : 0. ) + ( ks != k + 1 ? abs( e[ ks - 1 ] ) : 0. );
							if ( abs( p_sigma[ ks ] ) <= eps * t )
							{
								p_sigma[ ks ] = 0.0;
								break;
							}
						}
						if ( ks == k )
						{
							kase = 3;
						}
						else if ( ks == p - 1 )
						{
							kase = 1;
						}
						else
						{
							kase = 2;
							k	 = ks;
						}
					}
					k++;

					// Perform the task indicated by kase.

					switch ( kase )
					{
						// Deflate negligible s(p).

					case 1:
					{
						double f   = e[ p - 2 ];
						e[ p - 2 ] = 0.0;
						for ( j = p - 2; j >= k; j-- )
						{
							double t	 = hypot( p_sigma[ j ], f );
							double cs	 = p_sigma[ j ] / t;
							double sn	 = f / t;
							p_sigma[ j ] = t;
							if ( j != k )
							{
								f		   = -sn * e[ j - 1 ];
								e[ j - 1 ] = cs * e[ j - 1 ];
							}
							if ( wantv )
							{
								for ( i = 0; i < n; i++ )
								{
									t = cs * p_V.get( i, j ) + sn * p_V.get( i, p - 1 );
									p_V.set( i, p - 1, -sn * p_V.get( i, j ) + cs * p_V.get( i, p - 1 ) );
									p_V.set( i, j, t );
								}
							}
						}
					}
					break;

						// Split at negligible s(k).

					case 2:
					{
						double f   = e[ k - 1 ];
						e[ k - 1 ] = 0.0;
						for ( j = k; j < p; j++ )
						{
							double t	 = hypot( p_sigma[ j ], f );
							double cs	 = p_sigma[ j ] / t;
							double sn	 = f / t;
							p_sigma[ j ] = t;
							f			 = -sn * e[ j ];
							e[ j ]		 = cs * e[ j ];
							if ( wantu )
							{
								for ( i = 0; i < m; i++ )
								{
									t = cs * p_U.get( i, j ) + sn * p_U.get( i, k - 1 );
									p_U.set( i, k - 1, -sn * p_U.get( i, j ) + cs * p_U.get( i, k - 1 ) );
									p_U.set( i, j, t );
								}
							}
						}
					}
					break;

						// Perform one qr step.

					case 3:
					{
						// Calculate the shift.

						double scale = max(
							max( max( max( abs( p_sigma[ p - 1 ] ), abs( p_sigma[ p - 2 ] ) ), abs( e[ p - 2 ] ) ),
								 abs( p_sigma[ k ] ) ),
							abs( e[ k ] ) );
						double sp	 = p_sigma[ p - 1 ] / scale;
						double spm1	 = p_sigma[ p - 2 ] / scale;
						double epm1	 = e[ p - 2 ] / scale;
						double sk	 = p_sigma[ k ] / scale;
						double ek	 = e[ k ] / scale;
						double b	 = ( ( spm1 + sp ) * ( spm1 - sp ) + epm1 * epm1 ) / 2.0;
						double c	 = ( sp * epm1 ) * ( sp * epm1 );
						double shift = 0.0;
						if ( ( b != 0.0 ) || ( c != 0.0 ) )
						{
							shift = sqrt( b * b + c );
							if ( b < 0.0 )
							{
								shift = -shift;
							}
							shift = c / ( b + shift );
						}
						double f = ( sk + sp ) * ( sk - sp ) + shift;
						double g = sk * ek;

						// Chase zeros.

						for ( j = k; j < p - 1; j++ )
						{
							double t  = hypot( f, g );
							double cs = f / t;
							double sn = g / t;
							if ( j != k )
							{
								e[ j - 1 ] = t;
							}
							f				 = cs * p_sigma[ j ] + sn * e[ j ];
							e[ j ]			 = cs * e[ j ] - sn * p_sigma[ j ];
							g				 = sn * p_sigma[ j + 1 ];
							p_sigma[ j + 1 ] = cs * p_sigma[ j + 1 ];
							if ( wantv )
							{
								for ( i = 0; i < n; i++ )
								{
									t = cs * p_V.get( i, j ) + sn * p_V.get( i, j + 1 );
									p_V.set( i, j + 1, -sn * p_V.get( i, j ) + cs * p_V.get( i, j + 1 ) );
									p_V.set( i, j, t );
								}
							}
							t				 = hypot( f, g );
							cs				 = f / t;
							sn				 = g / t;
							p_sigma[ j ]	 = t;
							f				 = cs * e[ j ] + sn * p_sigma[ j + 1 ];
							p_sigma[ j + 1 ] = -sn * e[ j ] + cs * p_sigma[ j + 1 ];
							g				 = sn * e[ j + 1 ];
							e[ j + 1 ]		 = cs * e[ j + 1 ];
							if ( wantu && ( j < m - 1 ) )
							{
								for ( i = 0; i < m; i++ )
								{
									t = cs * p_U.get( i, j ) + sn * p_U.get( i, j + 1 );
									p_U.set( i, j + 1, -sn * p_U.get( i, j ) + cs * p_U.get( i, j + 1 ) );
									p_U.set( i, j, t );
								}
							}
						}
						e[ p - 2 ] = f;
						iter	   = iter + 1;
					}
					break;

						// Convergence.

					case 4:
					{
						// Make the singular values positive.

						if ( p_sigma[ k ] <= 0.0 )
						{
							p_sigma[ k ] = ( p_sigma[ k ] < 0.0 ? -p_sigma[ k ] : 0.0 );
							if ( wantv )
							{
								for ( i = 0; i <= pp; i++ )
								{
									p_V.set( i, k, -p_V.get( i, k ) );
								}
							}
						}

						// Order the singular values.

						while ( k < pp )
						{
							if ( p_sigma[ k ] >= p_sigma[ k + 1 ] )
							{
								break;
							}
							double t		 = p_sigma[ k ];
							p_sigma[ k ]	 = p_sigma[ k + 1 ];
							p_sigma[ k + 1 ] = t;
							if ( wantv && ( k < n - 1 ) )
							{
								for ( i = 0; i < n; i++ )
								{
									t = p_V.get( i, k + 1 );
									p_V.set( i, k + 1, p_V.get( i, k ) );
									p_V.set( i, k, t );
								}
							}
							if ( wantu && ( k < m - 1 ) )
							{
								for ( i = 0; i < m; i++ )
								{
									t = p_U.get( i, k + 1 );
									p_U.set( i, k + 1, p_U.get( i, k ) );
									p_U.set( i, k, t );
								}
							}
							k++;
						}
						iter = 0;
						p--;
					}
					break;
					}
				}
			}

			// Taken from https://math.nist.gov/tnt/download.html (JAMA/C++ Linear Algebra Package)
			template<typename T>
			inline void getLUDecomposition( const VTX::Math::Matrix<T> & m_mat,
											VTX::Math::Matrix<T> &		 p_lu,
											std::vector<T> &			 p_pivot,
											int &						 p_pivotSign )
			{
				const int m = int( m_mat.getRowCount() );
				const int n = int( m_mat.getColumnCount() );

				p_lu = VTX::Math::Matrix<T>( m_mat );

				p_pivot = std::vector<T>();
				p_pivot.resize( m );

				// Use a "left-looking", dot-product, Crout/Doolittle algorithm.
				for ( int i = 0; i < m; i++ )
				{
					p_pivot[ i ] = i;
				}

				p_pivotSign = 1;

				// Outer loop.

				for ( int j = 0; j < n; j++ )
				{
					// Apply previous transformations.

					for ( int i = 0; i < m; i++ )
					{
						// Most of the time is spent in the following dot product.

						const int kmax = min( i, j );
						T		  s	   = T( 0.0 );
						for ( int k = 0; k < kmax; k++ )
						{
							s += p_lu.get( i, k ) * p_lu.get( k, j );
						}

						p_lu.set( i, j, p_lu.get( i, j ) - s );
					}

					// Find pivot and exchange if necessary.

					int p = j;
					for ( int i = j + 1; i < m; i++ )
					{
						if ( abs( p_lu.get( i, j ) ) > abs( p_lu.get( p, j ) ) )
						{
							p = i;
						}
					}
					if ( p != j )
					{
						int k = 0;
						for ( k = 0; k < n; k++ )
						{
							const double t = p_lu.get( p, k );
							p_lu.set( p, k, p_lu.get( j, k ) );
							p_lu.set( j, k, t );
						}
						k			 = p_pivot[ p ];
						p_pivot[ p ] = p_pivot[ j ];
						p_pivot[ j ] = k;
						p_pivotSign	 = -p_pivotSign;
					}

					// Compute multipliers.

					if ( ( j < m ) && ( p_lu.get( j, j ) != 0.0 ) )
					{
						for ( int i = j + 1; i < m; i++ )
						{
							p_lu.set( i, j, p_lu.get( i, j ) / p_lu.get( j, j ) );
						}
					}
				}
			}

			// Taken from https://math.nist.gov/tnt/download.html (JAMA/C++ Linear Algebra Package)
			template<typename T>
			inline float getDeterminant( const VTX::Math::Matrix<T> & m_mat )
			{
				if ( m_mat.getRowCount() != m_mat.getColumnCount() )
				{
					return T( 0 );
				}

				T determinant = 1;
				for ( int i = 0; i < m_mat.getRowCount(); i++ )
				{
					determinant *= m_mat.get( i, i );
				}

				return determinant;
			}
		} // namespace Math

	} // namespace Util
} // namespace VTX

#endif
