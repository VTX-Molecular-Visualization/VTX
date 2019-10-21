/*
 * File: trackballCamera.hpp
 * Author: Maxime MARIA
 */

#ifndef __TRACKBALL_CAMERA_HPP__
#define __TRACKBALL_CAMERA_HPP__

#include "aCamera.hpp"

namespace vidocklab
{
	class TrackballCamera : public ACamera
	{
	public:
		TrackballCamera() = default;
		virtual ~TrackballCamera() = default;

	protected:
		float m_distance;
	};
} // namespace vidocklab

#endif // __TRACKBALL_CAMERA_HPP__
