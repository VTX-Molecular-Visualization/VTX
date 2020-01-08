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

	constexpr int	  OPENGL_VERSION_MAJOR = 4;
	constexpr int	  OPENGL_VERSION_MINOR = 5;
	const std::string GLSL_VERSION		   = "#version 450";
	const bool		  VSYNC				   = 0;

	// Window.
	constexpr int WINDOW_WIDTH	= 1280;
	constexpr int WINDOW_HEIGHT = 720;

	// Camera.
	constexpr float CAMERA_NEAR	   = 1e-1f;
	constexpr float CAMERA_FAR	   = 1e4f;
	constexpr float CAMERA_FOV	   = 60.f;
	constexpr float CAMERA_FOV_MIN = 10.f;
	constexpr float CAMERA_FOV_MAX = 90.f;

	// Auto rotate.
	constexpr float AUTO_ROTATE_SPEED_MIN		= 0.0f;
	constexpr float AUTO_ROTATE_SPEED_MAX		= 1.0f;
	constexpr float AUTO_ROTATE_X_SPEED_DEFAULT = AUTO_ROTATE_SPEED_MIN;
	constexpr float AUTO_ROTATE_Y_SPEED_DEFAULT = AUTO_ROTATE_SPEED_MIN;
	constexpr float AUTO_ROTATE_Z_SPEED_DEFAULT = AUTO_ROTATE_SPEED_MIN;

	// Dirs.
	const std::string DATA_DIR	   = "../data/";
	const std::string SHADER_DIR   = "../shaders/";
	const std::string SNAPSHOT_DIR = "../snapshots/";

	// Log.
	inline void	  VTX_DEBUG( const std::string & p_str ) { VTX::Util::Logger::logDebug( p_str ); }
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

	static float COLOR_BLACK[ 3 ]	   = { 0.f, 0.f, 0.f };
	static float COLOR_WHITE[ 3 ]	   = { 1.f, 1.f, 1.f };
	static float COLOR_GREY[ 3 ]	   = { 0.5f, 0.5f, 0.5f };
	static float COLOR_RED[ 3 ]		   = { 1.0f, 0.13f, 0.0f };
	static float COLOR_BLUE[ 3 ]	   = { 0.13f, 0.2f, 1.0f };
	static float COLOR_GREEN_DARK[ 3 ] = { 0.f, 0.47f, 0.f };
	static float COLOR_VIOLET[ 3 ]	   = { 0.f, 0.47f, 0.f };
	static float COLOR_ORANGE[ 3 ]	   = { 1.f, 0.5f, 0.f };
	static float COLOR_YELLOW[ 3 ]	   = { 1.f, 0.9f, 0.13f };
	static float COLOR_MAGENTA[ 3 ]	   = { 1.f, 0.f, 1.f };

	constexpr uint INVALID_ID = UINT_MAX;
} // namespace VTX

#endif
