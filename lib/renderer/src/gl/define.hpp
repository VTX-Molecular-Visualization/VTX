#ifndef __VTX_RENDERER_GL_DEFINE__
#define __VTX_RENDERER_GL_DEFINE__

#include <glm/glm.hpp>

namespace VTX
{
	// Types.
	using Vec2f = glm::vec2;
	using Vec2i = glm::ivec2;
	using Vec3f = glm::vec3;
	using Vec3d = glm::dvec3;
	using Vec3i = glm::ivec3;
	using Vec3u = glm::uvec3;
	using Vec4f = glm::vec4;
	using Vec4i = glm::ivec4;

	using Mat3f	 = glm::mat3;
	using Mat43f = glm::mat4x3;
	using Mat4f	 = glm::mat4;

	using Quatf = glm::quat;
	using Quatd = glm::dquat;

	// Mask.
	using VTX_RENDER_MASK = unsigned char;

	constexpr VTX_RENDER_MASK VTX_MASK_NEED_UPDATE		 = 0xFF;
	constexpr VTX_RENDER_MASK VTX_MASK_NO_UPDATE		 = 0x00;
	constexpr VTX_RENDER_MASK VTX_MASK_SELECTION_UPDATED = 0x01;
	constexpr VTX_RENDER_MASK VTX_MASK_CAMERA_UPDATED	 = 0x02;
	constexpr VTX_RENDER_MASK VTX_MASK_3D_MODEL_UPDATED	 = 0x04;
	constexpr VTX_RENDER_MASK VTX_MASK_UNIFORM_UPDATED	 = 0x08;
	constexpr VTX_RENDER_MASK VTX_MASK_FORCE_AA			 = 0x10;
} // namespace VTX

#endif
