#ifndef __VTX_DEFINES__
#define __VTX_DEFINES__

#ifdef _MSC_VER
#pragma once
#endif

#include "io/path.hpp"
#include "localization/language.hpp"
#include "util/logger.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <string>

namespace VTX
{
	// Types.
	using uint	= unsigned int;
	using uchar = unsigned char;

	using Vec2f = glm::vec2;
	using Vec2i = glm::ivec2;
	using Vec3f = glm::vec3;
	using Vec3d = glm::dvec3;
	using Vec4f = glm::vec4;

	using Mat3f = glm::mat3;
	using Mat3d = glm::dmat3;
	using Mat4f = glm::mat4;
	using Mat4d = glm::dmat4;

	using Quatf = glm::quat;
	using Quatd = glm::dquat;

	// VTX.
	constexpr int VTX_VERSION_MAJOR = 0;
	constexpr int VTX_VERSION_MINOR = 0;

	constexpr int	  OPENGL_VERSION_MAJOR = 4;
	constexpr int	  OPENGL_VERSION_MINOR = 5;
	const std::string GLSL_VERSION		   = "#version 450";
	const bool		  VSYNC				   = true;
	const bool		  RANDOM			   = false;

	const uint ACTION_BUFFER_SIZE = 10; // For undo/redo

	// Window.
	// constexpr int WINDOW_WIDTH	= 1280;
	// constexpr int WINDOW_HEIGHT = 720;
	// Full HD.
	constexpr int WINDOW_WIDTH	= 1920;
	constexpr int WINDOW_HEIGHT = 1080;
	// 4k
	// constexpr int WINDOW_WIDTH	= 3840;
	// constexpr int WINDOW_HEIGHT = 2160;
	// 4k with 250% scale
	// constexpr int WINDOW_WIDTH	= 1536;
	// constexpr int WINDOW_HEIGHT = 864;

	// Camera.
	constexpr float CAMERA_NEAR	   = 1e-1f;
	constexpr float CAMERA_FAR	   = 1e4f;
	constexpr float CAMERA_FOV	   = 60.f;
	constexpr float CAMERA_FOV_MIN = 10.f;
	constexpr float CAMERA_FOV_MAX = 90.f;

	// Controllers.
	constexpr float CONTROLLER_TRANSLATION_SPEED_MIN	  = 50.f;
	constexpr float CONTROLLER_TRANSLATION_SPEED_MAX	  = 300.f;
	constexpr float CONTROLLER_TRANSLATION_SPEED_DEFAULT  = 150.f;
	constexpr float CONTROLLER_TRANSLATION_FACTOR_MIN	  = 1.0f;
	constexpr float CONTROLLER_TRANSLATION_FACTOR_MAX	  = 5.0f;
	constexpr float CONTROLLER_TRANSLATION_FACTOR_DEFAULT = 2.0f;
	constexpr float CONTROLLER_ROTATION_SPEED_MIN		  = 0.001f;
	constexpr float CONTROLLER_ROTATION_SPEED_MAX		  = 0.01f;
	constexpr float CONTROLLER_ROTATION_SPEED_DEFAULT	  = 0.005f;
	constexpr float CONTROLLER_ORBIT_ELASTICITY			  = 8.0f;

	// Rendering.
	// TODO: choose relevent constants
	constexpr float RENDERER_AO_RADIUS_MIN		  = 0.f;
	constexpr float RENDERER_AO_RADIUS_MAX		  = 200.f;
	constexpr float RENDERER_AO_RADIUS_DEFAULT	  = 32.f;
	constexpr int	RENDERER_AO_INTENSITY_MIN	  = 1;
	constexpr int	RENDERER_AO_INTENSITY_MAX	  = 10;
	constexpr int	RENDERER_AO_INTENSITY_DEFAULT = 2;
	constexpr int	RENDERER_AO_BLUR_SIZE_MIN	  = 1;
	constexpr int	RENDERER_AO_BLUR_SIZE_MAX	  = 99;
	constexpr int	RENDERER_AO_BLUR_SIZE_DEFAULT = 9;

	// Path.
	constexpr float PATH_DEFAULT_DURATION = 5.f;

	// Export video.
	constexpr uint VIDEO_FPS = 60;

	// Auto rotate.
	constexpr float AUTO_ROTATE_SPEED_MIN		= 0.0f;
	constexpr float AUTO_ROTATE_SPEED_MAX		= 1.0f;
	constexpr float AUTO_ROTATE_X_SPEED_DEFAULT = AUTO_ROTATE_SPEED_MIN;
	constexpr float AUTO_ROTATE_Y_SPEED_DEFAULT = AUTO_ROTATE_SPEED_MIN;
	constexpr float AUTO_ROTATE_Z_SPEED_DEFAULT = AUTO_ROTATE_SPEED_MIN;

	// Dirs.
	const std::string DATA_DIR	   = "../data/";
	const IO::Path	  SHADER_DIR   = "../src/shaders/";
	const std::string SNAPSHOT_DIR = "../snapshots/";
	const std::string PATHS_DIR	   = "../paths/";
	const std::string VIDEO_DIR	   = "../videos/";

	// API.
	const std::string API_URL_MMTF = "http://mmtf.rcsb.org/v1.0/full/";

	// Log.
	inline void	  VTX_DEBUG( const std::string & p_str ) { VTX::Util::Logger::logDebug( p_str ); }
	inline void	  VTX_INFO( const std::string & p_str ) { VTX::Util::Logger::logInfo( p_str ); }
	inline void	  VTX_WARNING( const std::string & p_str ) { VTX::Util::Logger::logWarning( p_str ); }
	inline void	  VTX_ERROR( const std::string & p_str ) { VTX::Util::Logger::logError( p_str ); }
	constexpr int VTX_CONSOLE_SIZE = 80;

	// Localization.
	inline const char * LOCALE( const char * p_str ) { return VTX::Localization::Language::text( p_str ); }

	// Constants.
	const Mat4f MAT4F_ID = Mat4f( 1.f );

	const Vec3f VEC3F_ZERO = Vec3f( 0.f, 0.f, 0.f );
	const Vec3d VEC3D_ZERO = Vec3d( 0.0, 0.0, 0.0 );
	const Vec3f VEC3F_X	   = Vec3f( 1.f, 0.f, 0.f );
	const Vec3f VEC3F_Y	   = Vec3f( 0.f, 1.f, 0.f );
	const Vec3f VEC3F_Z	   = Vec3f( 0.f, 0.f, 1.f );
	const Vec3f VEC3F_XYZ  = Vec3f( 1.f, 1.f, 1.f );

	const float PIf	   = glm::pi<float>();
	const float PI_2f  = glm::half_pi<float>();
	const float PI_32f = glm::three_over_two_pi<float>();

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
