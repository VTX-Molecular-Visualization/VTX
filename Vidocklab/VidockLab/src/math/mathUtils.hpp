/*
 * File: mathUtils.hpp
 * Author: Maxime MARIA
 */

#ifndef __MATH_UTILS_HPP__
#define __MATH_UTILS_HPP__

#include "defines.hpp"

#include <random>

namespace vidocklab
{
	static std::random_device rd;
	static std::mt19937 gen(79);
	// rd()); // Standard mersenne_twister_engine seeded with rd()
	static std::uniform_real_distribution<float> dis(0.f, 1.f);

	inline Vec3f min(const Vec3f &a, const Vec3f &b)
	{
		return Vec3f(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y,
					 a.z < b.z ? a.z : b.z);
	}

	inline Vec3f max(const Vec3f &a, const Vec3f &b)
	{
		return Vec3f(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y,
					 a.z > b.z ? a.z : b.z);
	}

	inline Vec3f rndVec3f()
	{
		return Vec3f(dis(gen), dis(gen), dis(gen));
	}

} // namespace vidocklab

#endif // __MATH_UTILS_HPP__
