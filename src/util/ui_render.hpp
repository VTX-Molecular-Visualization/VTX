#ifndef __VTX_UTIL_UI_RENDER__
#define __VTX_UTIL_UI_RENDER__

#include "define.hpp"
#include <QPoint>
#include <QRect>
#include <vector>

namespace Object3D
{
	class Camera;
}

namespace VTX::Util::UIRender
{
	QPoint worldToScreen( const Vec3f & p_worldPos, const Object3D::Camera & p_camera, const QRect & p_renderRect );
	void   fillScreenPositions( const std::vector<Vec3f> & p_worldPositions,
								std::vector<QPoint> &	   p_screenPositions,
								const Object3D::Camera &   p_camera,
								const QRect &			   p_renderRect );

	Vec3f getCenter( const std::vector<Vec3f> & p_worldPositions );

	void getMinMax( const std::vector<QPoint> & p_positions, int & p_minX, int & p_maxX, int & p_minY, int & p_maxY );

	bool isVisibleToCamera( const Object3D::Camera & p_camera, const Vec3f & p_worldPosition );
	bool anyVisibleToCamera( const Object3D::Camera & p_camera, const std::vector<Vec3f> & p_worldPositions );

	float distanceToCamera( const Object3D::Camera & p_camera, const Vec3f & p_worldPos );

	template<typename T>
	inline T linearInterpolation( const T & p_min, const T & p_max, const float p_ratio )
	{
		return p_min + ( p_max - p_min ) * p_ratio;
	}

} // namespace VTX::Util::UIRender

#endif
