/*
 * File: aabb.hpp
 * Author: Maxime MARIA
 */

#ifndef __AABB_HPP__
#define __AABB_HPP__

#include "glm/glm.hpp"

#include "defines.hpp"
#include "math/mathUtils.hpp"

namespace vidocklab
{
	class AABB
	{
	public:
		AABB() = default;
		~AABB() = default;

		explicit AABB(const Vec3f &pt) : m_min(pt), m_max(pt) {}
		AABB(const Vec3f &center, const float radius)
			: m_min(center - radius), m_max(center - radius)
		{
		}

		const Vec3f &getMin() const
		{
			return m_min;
		}

		const Vec3f &getMax() const
		{
			return m_max;
		}

		void extend(const Vec3f &pt)
		{
			m_min = min(pt, m_min);
			m_max = max(pt, m_max);
		}

		void extend(const Vec3f &center, const float radius)
		{
			extend(center - radius);
			extend(center + radius);
		}

		Vec3f center() const
		{
			return (m_min + m_max) * 0.5f;
		}

		float radius() const
		{
			return glm::length(m_max - m_min) * 0.5f;
		}

	private:
		Vec3f m_min = Vec3f(FLT_MAX);
		Vec3f m_max = Vec3f(-FLT_MAX);
	};
} // namespace vidocklab

#endif // __AABB_HPP__
