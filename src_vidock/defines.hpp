/*
 * File: defines.hpp
 * Author: Maxime MARIA
 */

#ifndef __DEFINES_HPP__
#define __DEFINES_HPP__

#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

namespace vidocklab
{
	using uint = unsigned int;

	using Vec2f = glm::vec2;
	using Vec3f = glm::vec3;
	using Vec4f = glm::vec4;

	using Mat4f = glm::mat4;

	const std::string UNDEFINED_STRING = "undefined";
	const std::string DONE_STRING = "[DONE]";

	const Mat4f MAT4F_ID = Mat4f(1.f);
	const Vec3f VEC3F_ZERO = Vec3f(0.f, 0.f, 0.f);
	const Vec3f VEC3F_X = Vec3f(1.f, 0.f, 0.f);
	const Vec3f VEC3F_Y = Vec3f(0.f, 1.f, 0.f);
	const Vec3f VEC3F_Z = Vec3f(0.f, 0.f, 1.f);
	const float PIf = glm::pi<float>();
	const float PI_2f = glm::half_pi<float>();

	const uint INVALID_ID = UINT_MAX;
} // namespace vidocklab

#endif // __DEFINES_HPP__
