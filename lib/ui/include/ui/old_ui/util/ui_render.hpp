#ifndef __VTX_UTIL_UI_RENDER__
#define __VTX_UTIL_UI_RENDER__

#include <QLine>
#include <QPoint>
#include <QRect>
#include <QVector2D>
#include <app/component/render/_fwd.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Util::UIRender
{
	inline QPoint	 vec3fToQPoint( const Vec3f & p_vec3f ) { return QPoint( p_vec3f.x, p_vec3f.y ); }
	inline QVector2D vec3fToQVector2D( const Vec3f & p_vec3f ) { return QVector2D( p_vec3f.x, p_vec3f.y ); }
	inline Vec3f	 qPointToVec3f( const QPoint & p_point ) { return Vec3f( p_point.x(), p_point.y(), 0 ); }

	QPoint worldToScreenQPoint( const Vec3f &							 p_worldPos,
								const App::Component::Render::Camera & p_camera,
								const QRect &							 p_renderRect );
	Vec3f  worldToScreenVec3f( const Vec3f &							p_worldPos,
							   const App::Component::Render::Camera & p_camera,
							   const QRect &							p_renderRect );

	Vec3f screenToWorldVector( const QPoint &							p_screenPos,
							   const App::Component::Render::Camera & p_camera,
							   const QRect &							p_renderRect );
	bool  isInSphere( const Vec3f &							   p_worldSphereCenter,
					  const float							   p_sphereRadius,
					  const QPoint &						   p_screenPos,
					  const App::Component::Render::Camera & p_camera,
					  const QRect &							   p_renderRect );

	void fillScreenPositions( const std::vector<Vec3f> &			   p_worldPositions,
							  std::vector<QPoint> &					   p_screenPositions,
							  const App::Component::Render::Camera & p_camera,
							  const QRect &							   p_renderRect );
	void fillScreenPositions( const std::vector<Vec3f> &			   p_worldPositions,
							  std::vector<Vec3f> &					   p_screenPositions,
							  const App::Component::Render::Camera & p_camera,
							  const QRect &							   p_renderRect );

	Vec3f getCenter( const std::vector<Vec3f> & p_worldPositions );

	void getMinMax( const std::vector<QPoint> & p_positions, int & p_minX, int & p_maxX, int & p_minY, int & p_maxY );

	bool isVisibleToCamera( const App::Component::Render::Camera & p_camera, const Vec3f & p_worldPosition );
	bool anyVisibleToCamera( const App::Component::Render::Camera & p_camera,
							 const std::vector<Vec3f> &				  p_worldPositions );

	float distanceToCamera( const App::Component::Render::Camera & p_camera, const Vec3f & p_worldPos );

	QLine getScreenLine( const Vec3f & p_point1, const Vec3f & p_point2 );

	template<typename T>
	inline T linearInterpolation( const T & p_min, const T & p_max, const float p_ratio )
	{
		return p_min + ( p_max - p_min ) * p_ratio;
	}

} // namespace VTX::Util::UIRender

#endif
