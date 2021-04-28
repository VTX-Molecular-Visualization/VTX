#ifndef __VTX_DEFINE__
#define __VTX_DEFINE__

#ifdef _MSC_VER
#pragma once
#endif

#include <filesystem>
#include <glm/detail/qualifier.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <limits>
#include <string>

namespace VTX
{
	// Types.
	using ushort = unsigned short;
	using uint	 = unsigned int;
	using ulong	 = unsigned long;
	using uchar	 = unsigned char;

	using Vec2f = glm::vec2;
	using Vec2i = glm::ivec2;
	using Vec3f = glm::vec3;
	using Vec3d = glm::dvec3;
	using Vec3i = glm::ivec3;
	using Vec4f = glm::vec4;
	using Vec4i = glm::ivec4;

	using Mat3f	 = glm::mat3;
	using Mat43f = glm::mat4x3;
	using Mat4f	 = glm::mat4;

	using Quatf = glm::quat;
	using Quatd = glm::dquat;

	using FilePath = std::filesystem::path;

	// VTX.
	constexpr int VTX_VERSION_MAJOR	   = 0;
	constexpr int VTX_VERSION_MINOR	   = 1;
	constexpr int VTX_VERSION_REVISION = 0;

	constexpr int	  OPENGL_VERSION_MAJOR = 4;
	constexpr int	  OPENGL_VERSION_MINOR = 5;
	const std::string GLSL_VERSION		   = "#version 450";

	// API.
	const std::string API_URL_MMTF = "http://mmtf.rcsb.org/v1.0/full/";

	// Constants.
	constexpr float FLOAT_MIN = std::numeric_limits<float>::lowest();
	constexpr float FLOAT_MAX = std::numeric_limits<float>::max();
	constexpr float FLOAT_INF = std::numeric_limits<float>::infinity();

	constexpr float PIf		 = glm::pi<float>();
	constexpr float PI_2f	 = glm::half_pi<float>();
	constexpr float PI_4f	 = glm::quarter_pi<float>();
	constexpr float PI_32f	 = glm::three_over_two_pi<float>();
	constexpr float TWO_PIf	 = glm::two_pi<float>();
	constexpr float INV_PIf	 = glm::one_over_pi<float>();
	constexpr float INV_2PIf = glm::one_over_two_pi<float>();

	constexpr Vec3f VEC3F_ZERO = Vec3f( 0.f, 0.f, 0.f );
	constexpr Vec3f VEC3F_X	   = Vec3f( 1.f, 0.f, 0.f );
	constexpr Vec3f VEC3F_Y	   = Vec3f( 0.f, 1.f, 0.f );
	constexpr Vec3f VEC3F_Z	   = Vec3f( 0.f, 0.f, 1.f );
	constexpr Vec3f VEC3F_XYZ  = Vec3f( 1.f, 1.f, 1.f );
	constexpr Vec3f VEC3F_MIN  = Vec3f( FLOAT_MIN );
	constexpr Vec3f VEC3F_MAX  = Vec3f( FLOAT_MAX );

	constexpr Mat3f MAT3F_ID = Mat3f( 1.f );
	constexpr Mat4f MAT4F_ID = Mat4f( 1.f );

	constexpr Quatf QUATF_ID = Quatf( 1.f, { 0.f, 0.f, 0.f } );

	constexpr Vec3f CAMERA_RIGHT_DEFAULT = VEC3F_X;
	constexpr Vec3f CAMERA_UP_DEFAULT	 = VEC3F_Y;
	constexpr Vec3f CAMERA_FRONT_DEFAULT = Vec3f( 0.f, 0.f, -1.f );

	constexpr uint INVALID_ID = UINT_MAX;

	// Mask.
	using VTX_MASK = unsigned char;

	constexpr VTX_MASK VTX_MASK_NEED_UPDATE		  = 0xFF;
	constexpr VTX_MASK VTX_MASK_NO_UPDATE		  = 0x00;
	constexpr VTX_MASK VTX_MASK_SELECTION_UPDATED = 0x01;
	constexpr VTX_MASK VTX_MASK_CAMERA_UPDATED	  = 0x02;
	constexpr VTX_MASK VTX_MASK_3D_MODEL_UPDATED  = 0x04;
	constexpr VTX_MASK VTX_MASK_UNIFORM_UPDATED	  = 0x08;
} // namespace VTX

#endif
