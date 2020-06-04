#ifndef __VTX_DEFINE__
#define __VTX_DEFINE__

#ifdef _MSC_VER
#pragma once
#endif

#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/spline.hpp>
#include <limits>
#include <string>

namespace VTX
{
	// Types.
	using uint	= unsigned int;
	using uchar = unsigned char;

	using Vec2f = glm::vec2;
	using Vec2i = glm::ivec2;
	using Vec3f = glm::vec3;
	using Vec3i = glm::ivec3;
	using Vec3d = glm::dvec3;
	using Vec4f = glm::vec4;

	using Mat3f	 = glm::mat3;
	using Mat43f = glm::mat4x3;
	using Mat3d	 = glm::dmat3;
	using Mat4f	 = glm::mat4;
	using Mat4d	 = glm::dmat4;

	using Quatf = glm::quat;
	using Quatd = glm::dquat;

	using Path = std::filesystem::path;

	// VTX.
	constexpr int VTX_VERSION_MAJOR	   = 0;
	constexpr int VTX_VERSION_MINOR	   = 0;
	constexpr int VTX_VERSION_REVISION = 5;

	constexpr int	  OPENGL_VERSION_MAJOR = 4;
	constexpr int	  OPENGL_VERSION_MINOR = 5;
	const std::string GLSL_VERSION		   = "#version 450";
	const bool		  VSYNC_DEFAULT		   = true;

	const bool SSAO_DEFAULT			 = true;
	const bool CONTOUR_LINES_DEFAULT = false; // Not implemented yet.
	const bool AA_DEFAULT			 = true;
	const bool RANDOM				 = false; // Keep it false for dev branch.

	const uint ACTION_BUFFER_SIZE = 10; // For undo/redo

	// Window.
	// 360p.
	// constexpr int WINDOW_WIDTH	= 640;
	// constexpr int WINDOW_HEIGHT = 320;
	// 720p.
	constexpr int WINDOW_WIDTH	= 1280;
	constexpr int WINDOW_HEIGHT = 720;
	// Full HD.
	// constexpr int WINDOW_WIDTH	= 1920;
	// constexpr int WINDOW_HEIGHT = 1080;
	// 4k
	// constexpr int WINDOW_WIDTH	= 3840;
	// constexpr int WINDOW_HEIGHT = 2160;
	// 4k with 250% scale
	// constexpr int WINDOW_WIDTH	= 1536;
	// constexpr int WINDOW_HEIGHT = 864;

	// Camera.
	constexpr float CAMERA_NEAR_DEFAULT = 0.f;
	constexpr float CAMERA_FAR_DEFAULT	= 1e4f;
	constexpr float CAMERA_NEAR_FAR_MIN = 0.f;
	constexpr float CAMERA_NEAR_FAR_MAX = 1e4f;
	constexpr float CAMERA_FOV_DEFAULT	= 60.f;
	constexpr float CAMERA_FOV_MIN		= 10.f;
	constexpr float CAMERA_FOV_MAX		= 90.f;

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
	// TODO: choose relevant constants
	constexpr int	RENDERER_AO_INTENSITY_MIN		   = 1;
	constexpr int	RENDERER_AO_INTENSITY_MAX		   = 10;
	constexpr int	RENDERER_AO_INTENSITY_DEFAULT	   = 5;
	constexpr float RENDERER_AO_FACTOR_DEFAULT		   = 0.5f;
	constexpr int	RENDERER_AO_BLUR_SIZE_MIN		   = 1;
	constexpr int	RENDERER_AO_BLUR_SIZE_MAX		   = 99;
	constexpr int	RENDERER_AO_BLUR_SIZE_DEFAULT	   = 9;
	constexpr int	RENDERER_AO_BLUR_SHARPNESS_MIN	   = 1;
	constexpr int	RENDERER_AO_BLUR_SHARPNESS_MAX	   = 32;
	constexpr int	RENDERER_AO_BLUR_SHARPNESS_DEFAULT = 8;

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

	// API.
	const std::string API_URL_MMTF = "http://mmtf.rcsb.org/v1.0/full/";

	// Log.
	constexpr int VTX_CONSOLE_SIZE = 80;

	// Constants.
	const float FLOAT_MIN = std::numeric_limits<float>::lowest();
	const float FLOAT_MAX = std::numeric_limits<float>::max();
	const float FLOAT_INF = std::numeric_limits<float>::infinity();

	const float PIf		 = glm::pi<float>();
	const float PI_2f	 = glm::half_pi<float>();
	const float PI_4f	 = glm::quarter_pi<float>();
	const float PI_32f	 = glm::three_over_two_pi<float>();
	const float TWO_PIf	 = glm::two_pi<float>();
	const float INV_PIf	 = glm::one_over_pi<float>();
	const float INV_2PIf = glm::one_over_two_pi<float>();

	const Vec3f VEC3F_ZERO = Vec3f( 0.f, 0.f, 0.f );
	const Vec3d VEC3D_ZERO = Vec3d( 0.0, 0.0, 0.0 );
	const Vec3f VEC3F_X	   = Vec3f( 1.f, 0.f, 0.f );
	const Vec3f VEC3F_Y	   = Vec3f( 0.f, 1.f, 0.f );
	const Vec3f VEC3F_Z	   = Vec3f( 0.f, 0.f, 1.f );
	const Vec3f VEC3F_XYZ  = Vec3f( 1.f, 1.f, 1.f );
	const Vec3f VEC3F_MIN  = Vec3f( FLOAT_MIN );
	const Vec3f VEC3F_MAX  = Vec3f( FLOAT_MAX );

	const Mat3f MAT3F_ID = Mat3f( 1.f );
	const Mat4f MAT4F_ID = Mat4f( 1.f );

	constexpr uint INVALID_ID = UINT_MAX;
} // namespace VTX

#endif
