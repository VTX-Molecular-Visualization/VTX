#ifndef __VTX__MATH__
#define __VTX__MATH__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "model/model_path.hpp"
#include "object3d/camera.hpp"
#include <glm/gtx/compatibility.hpp>
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

			static inline Object3D::Camera::CameraConfiguration lerpCameraConfiguration(
				Model::ModelPath * const p_path,
				uint					 p_offset,
				float					 p_value )
			{
				float theta	   = glm::lerp( p_path->getCheckpoints()[ p_offset > 0 ? p_offset - 1 : 0 ]->getTheta(),
											p_path->getCheckpoints()[ p_offset ]->getTheta(),
											p_value );
				float phi	   = glm::lerp( p_path->getCheckpoints()[ p_offset > 0 ? p_offset - 1 : 0 ]->getPhi(),
										p_path->getCheckpoints()[ p_offset ]->getPhi(),
										p_value );
				Vec3f position = glm::lerp( p_path->getCheckpoints()[ p_offset > 0 ? p_offset - 1 : 0 ]->getPosition(),
											p_path->getCheckpoints()[ p_offset ]->getPosition(),
											p_value );

				struct Object3D::Camera::CameraConfiguration config;
				config.position = position;
				config.theta	= theta;
				config.phi		= phi;

				return config;
			}

		} // namespace Math
	}	  // namespace Util
} // namespace VTX

#endif
