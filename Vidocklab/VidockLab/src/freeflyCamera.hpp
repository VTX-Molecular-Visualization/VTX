/*
 * File: freeflyCamera.hpp
 * Author: Maxime MARIA
 */

#ifndef __FREEFLY_CAMERA_HPP__
#define __FREEFLY_CAMERA_HPP__

#include "aCamera.hpp"

#include <iostream>

#include "glm/gtc/matrix_transform.hpp"

#include "defines.hpp"
#include "io/ioUtils.hpp"

namespace vidocklab
{
	class FreeflyCamera : public ACamera
	{
	public:
		FreeflyCamera() : ACamera()
		{
			update();
		}

		void setPosition(const Vec3f &pos)
		{
			m_pos = pos;
		}

		void setFront(const Vec3f &front)
		{
			m_front = glm::normalize(front);
			update();
		}

		virtual void moveFront(const float delta) override
		{
			m_pos += delta * m_front;
		}

		virtual void moveLeft(const float delta) override
		{
			m_pos += delta * m_left;
		}

		virtual void moveUp(const float delta) override
		{
			m_pos += delta * m_up;
		}

		virtual void rotateLeft(const float angle) override
		{
			m_phi -= glm::radians(angle);
			update();
		}

		virtual void rotateUp(const float angle) override
		{
			m_theta -= glm::radians(angle);
			update();
		}

		virtual Mat4f getViewMatrix() const override
		{
			return glm::lookAt(m_pos, m_pos + m_front, m_up);
		}

		virtual void printInfo() const override
		{
			std::cout << "Type: Freefly" << std::endl;
			std::cout << "Position : " << m_pos << std::endl;
			std::cout << "Front    : " << m_front << std::endl;
			std::cout << "Left     : " << m_left << std::endl;
			std::cout << "Up       : " << m_up << std::endl;
			std::cout << "Phi      : " << m_phi << std::endl;
			std::cout << "Theta    : " << m_theta << std::endl;
			std::cout << "Fov      : " << m_fov << std::endl;
		}

		const Vec3f &getPosition() const
		{
			return m_pos;
		}

	private:
		virtual void update()
		{
			const float cosTheta = cosf(m_theta);
			m_front = Vec3f(cosTheta * sinf(m_phi), sinf(m_theta),
							cosTheta * cosf(m_phi));
			m_left = Vec3f(sinf(m_phi + PI_2f), 0.f, cosf(m_phi + PI_2f));
			m_up = glm::cross(m_front, m_left);
		}

	protected:
		/*Position : [ 806.803, 1121.98, 712.788 ] Front
			: [ 0.387621, -0.64011, -0.663332 ] Left
			: [ -0.863395, 0, -0.504528 ] Up
			: [ 0.322953, 0.768283, -0.552668 ] Phi : 2.61276 Theta
			: -0.694641 Fov : 60;*/
		Vec3f m_pos
			//= VEC3F_ZERO;
			//= Vec3f(-1.9233e-07, 0, -2.2);
			//= Vec3f(28.6974, 107.456, -72.8066);
			//= Vec3f(25.3197f, 928.787f, -1023.249f);
			= Vec3f(1444.52, 582.281, 990.344);
		//= Vec3f(377.618, 1228.58, 0.0116177);
		//= Vec3f(697.252, 199.743, 1706.77); // pdv video
		// Vec3f(773.704, 1184.4, 678.455);
		// Vec3f(827.962, 1173.53, 784.935); // 0.f,
		// 0.f, 5.f); // Vec3f(806.803, 1121.98, 712.788);
		Vec3f m_front // = VEC3F_Z;
			= Vec3f(-0.888385, -0.0226873, -0.458538);
		Vec3f m_left = VEC3F_X;
		Vec3f m_up = VEC3F_Y;
	};
} // namespace vidocklab

#endif // __FREEFLY_CAMERA_HPP__
