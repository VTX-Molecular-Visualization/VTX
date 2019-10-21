/*
 * File: modelMatrix.hpp
 * Author: Maxime MARIA
 */

#ifndef __MODEL_MATRIX_HPP__
#define __MODEL_MATRIX_HPP__

#include "glm/gtc/matrix_transform.hpp"

#include "defines.hpp"

namespace vidocklab
{
	class ModelMatrix
	{
	public:
		ModelMatrix() = default;
		~ModelMatrix() = default;

		const Mat4f &getModel() const
		{
			return m_model;
		}

		void reset()
		{
			m_model = MAT4F_ID;
			m_scale = MAT4F_ID;
			m_translate = MAT4F_ID;
			m_rotate = MAT4F_ID;
		}

		void translate(const Vec3f &v)
		{
			m_translate = glm::translate(m_translate, v);
			update();
		}

		void rotate(const float angle, const Vec3f &axis)
		{
			m_rotate = glm::rotate(m_rotate, angle, axis);
			update();
		}

		void scale(const Vec3f &s)
		{
			m_scale = glm::scale(m_scale, s);
			update();
		}

	private:
		void update()
		{
			m_model = m_translate * m_rotate * m_scale;
		}

	private:
		Mat4f m_model = MAT4F_ID;
		Mat4f m_translate = MAT4F_ID;
		Mat4f m_rotate = MAT4F_ID;
		Mat4f m_scale = MAT4F_ID;
	};
} // namespace vidocklab

#endif // __MODEL_MATRIX_HPP__
