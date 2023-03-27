#ifndef __VTX_RENDERER_GL_TYPES__
#define __VTX_RENDERER_GL_TYPES__

#include <glm/detail/qualifier.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

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
	using Vec3u = glm::uvec3;
	using Vec4f = glm::vec4;
	using Vec4i = glm::ivec4;
	using Vec4u = glm::uvec4;

	using Mat3f	 = glm::mat3;
	using Mat43f = glm::mat4x3;
	using Mat4f	 = glm::mat4;

	using Quatf = glm::quat;
	using Quatd = glm::dquat;
} // namespace VTX

#endif
