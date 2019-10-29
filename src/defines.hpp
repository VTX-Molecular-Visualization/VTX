#ifndef __DEFINES__
#define __DEFINES__

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <string>

namespace VTX
{
	constexpr int VTX_VERSION_MAJOR = 0;
	constexpr int VTX_VERSION_MINOR = 0;
	constexpr int VTX_VERSION_RELEASE = 0;

	constexpr int OPENGL_VERSION_MAJOR = 4;
	constexpr int OPENGL_VERSION_MINOR = 5;

	using uint = unsigned int;
	using uchar = unsigned char;

	using Vec2f = glm::vec2;
	using Vec3f = glm::vec3;
	using Vec4f = glm::vec4;

	using Mat4f = glm::mat4;

	const Mat4f MAT4F_ID   = Mat4f( 1.f );
	const Vec3f VEC3F_ZERO = Vec3f( 0.f, 0.f, 0.f );
	const Vec3f VEC3F_X	   = Vec3f( 1.f, 0.f, 0.f );
	const Vec3f VEC3F_Y	   = Vec3f( 0.f, 1.f, 0.f );
	const Vec3f VEC3F_Z	   = Vec3f( 0.f, 0.f, 1.f );
	const float PIf		   = glm::pi<float>();
	const float PI_2f	   = glm::half_pi<float>();

	const uint INVALID_ID = UINT_MAX;
} // namespace VTX

#endif
