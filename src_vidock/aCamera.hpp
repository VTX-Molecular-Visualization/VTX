/*
 * File: aCamera.hpp
 * Author: Maxime MARIA
 */

#ifndef __A_CAMERA_HPP__
#define __A_CAMERA_HPP__

#include "glm/gtc/matrix_transform.hpp"

#include "defines.hpp"

namespace vidocklab
{
	class ACamera
	{
	public:
		ACamera() = default;
		virtual ~ACamera() = default;

		virtual void moveFront(const float delta) = 0;
		virtual void moveLeft(const float delta) = 0;
		virtual void moveUp(const float delta) = 0;
		virtual void rotateLeft(const float angle) = 0;
		virtual void rotateUp(const float angle) = 0;

		virtual Mat4f getViewMatrix() const = 0;

		inline Mat4f getProjectionMatrix() const;
		inline void zoom(const float f);
		virtual void printInfo() const = 0;

		inline void setScreenSize(const int width, const int height);

	protected:
		float m_screenWidth = 1280.f;
		float m_screenHeight = 720.f;
		float m_near = 1e-1f;
		float m_far = 1e4f;
		float m_fov = 60.f;
		float m_theta = 0.f;

		float m_phi = 4.22369;
		// PIf;

		static const float MIN_FOV;
		static const float MAX_FOV;
	};

	inline Mat4f ACamera::getProjectionMatrix() const
	{
		return glm::perspective(glm::radians(m_fov),
								m_screenWidth / m_screenHeight, m_near, m_far);
	}

	// TODO: change name
	inline void ACamera::zoom(const float f)
	{
		m_fov = glm::clamp(m_fov - f, MIN_FOV, MAX_FOV);
	}

	inline void ACamera::setScreenSize(const int width, const int height)
	{
		m_screenWidth = float(width);
		m_screenHeight = float(height);
	}
} // namespace vidocklab

#endif // __CAMERA_HPP__
