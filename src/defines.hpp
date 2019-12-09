#ifndef __VTX_DEFINES__
#define __VTX_DEFINES__

#ifdef _MSC_VER
#pragma once
#endif

#include "localization/language.hpp"
#include "util/logger.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <string>

namespace VTX
{
	// VTX CONFIGURATION.
	constexpr int VTX_VERSION_MAJOR = 0;
	constexpr int VTX_VERSION_MINOR = 0;

	constexpr int OPENGL_VERSION_MAJOR = 4;
	constexpr int OPENGL_VERSION_MINOR = 5;

	constexpr int WINDOWS_WIDTH	 = 800;
	constexpr int WINDOWS_HEIGHT = 600;

	constexpr float CAMERA_NEAR	   = 1e-1f;
	constexpr float CAMERA_FAR	   = 1e4f;
	constexpr float CAMERA_FOV	   = 60.f;
	constexpr float CAMERA_FOV_MIN = 10.f;
	constexpr float CAMERA_FOV_MAX = 90.f;

	const std::string DATA_DIR	 = "../data/";
	const std::string SHADER_DIR = "../shaders/";

	// Log.
	inline void	  VTX_DEBUG( const std::string & p_str ) { VTX::Util::Logger::logInfo( p_str ); }
	inline void	  VTX_INFO( const std::string & p_str ) { VTX::Util::Logger::logInfo( p_str ); }
	inline void	  VTX_WARNING( const std::string & p_str ) { VTX::Util::Logger::logWarning( p_str ); }
	inline void	  VTX_ERROR( const std::string & p_str ) { VTX::Util::Logger::logError( p_str ); }
	constexpr int VTX_CONSOLE_SIZE = 80;

	// Localization.
	inline const char * LOCALE( const char * p_str ) { return VTX::Localization::Language::text( p_str ); }

	// Types.
	using uint	= unsigned int;
	using uchar = unsigned char;

	using Vec2f = glm::vec2;
	using Vec3f = glm::vec3;
	using Vec4f = glm::vec4;

	using Mat4f = glm::mat4;

	// Constants.
	const Mat4f MAT4F_ID = Mat4f( 1.f );

	const Vec3f VEC3F_ZERO = Vec3f( 0.f, 0.f, 0.f );
	const Vec3f VEC3F_X	   = Vec3f( 1.f, 0.f, 0.f );
	const Vec3f VEC3F_Y	   = Vec3f( 0.f, 1.f, 0.f );
	const Vec3f VEC3F_Z	   = Vec3f( 0.f, 0.f, 1.f );
	const Vec3f VEC3F_XYZ  = Vec3f( 1.f, 1.f, 1.f );

	const float PIf	  = glm::pi<float>();
	const float PI_2f = glm::half_pi<float>();

	const Vec3f COLOR_BLACK		 = VEC3F_ZERO;
	const Vec3f COLOR_WHITE		 = VEC3F_XYZ;
	const Vec3f COLOR_GREY		 = Vec3f( 0.5f, 0.5f, 0.5f );
	const Vec3f COLOR_RED		 = Vec3f( 1.0f, 0.13f, 0.0f );
	const Vec3f COLOR_BLUE		 = Vec3f( 0.13f, 0.2f, 1.0f );
	const Vec3f COLOR_GREEN_DARK = Vec3f( 0.f, 0.47f, 0.f );
	const Vec3f COLOR_VIOLET	 = Vec3f( 0.f, 0.47f, 0.f );
	const Vec3f COLOR_ORANGE	 = Vec3f( 1.f, 0.5f, 0.f );
	const Vec3f COLOR_YELLOW	 = Vec3f( 1.f, 0.9f, 0.13f );
	const Vec3f COLOR_MAGENTA	 = Vec3f( 1.f, 0.f, 1.f );

	constexpr uint INVALID_ID = UINT_MAX;
} // namespace VTX

#endif
