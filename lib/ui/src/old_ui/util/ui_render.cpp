#include "ui/old_ui/util/ui_render.hpp"
#include <app/component/render/camera.hpp>

namespace VTX::Util::UIRender
{
	QPoint worldToScreenQPoint( const Vec3f &								p_worldPos,
								const VTX::App::Component::Render::Camera & p_camera,
								const QRect &								p_renderRect )
	{
		const QPoint renderPos = QPoint( p_renderRect.width() / 2, p_renderRect.height() / 2 );

		const Mat4f & projectionMatrix = p_camera.getProjectionMatrix();
		const Mat4f & viewMatrix	   = p_camera.getViewMatrix();
		const Vec4f	  screenPos
			= projectionMatrix * ( viewMatrix * Vec4f( p_worldPos.x, p_worldPos.y, p_worldPos.z, 1.f ) );

		QPoint screenPointPos;
		if ( screenPos.w != 0 )
		{
			const float absW		= screenPos.w < 0 ? -screenPos.w : screenPos.w;
			const Vec3f viewportPos = Vec3f( screenPos ) / absW;
			screenPointPos			= QPoint( ( viewportPos.x + 1.f ) * 0.5f * p_renderRect.width(),
									  ( -viewportPos.y + 1.f ) * 0.5f * p_renderRect.height() );
		}
		else
		{
			screenPointPos = { -1000, -1000 };
		}

		return screenPointPos;
	}
	Vec3f worldToScreenVec3f( const Vec3f &								  p_worldPos,
							  const VTX::App::Component::Render::Camera & p_camera,
							  const QRect &								  p_renderRect )
	{
		const QPoint renderPos = QPoint( p_renderRect.width() / 2, p_renderRect.height() / 2 );

		const Mat4f & projectionMatrix = p_camera.getProjectionMatrix();
		const Mat4f & viewMatrix	   = p_camera.getViewMatrix();
		const Vec4f	  screenPos
			= projectionMatrix * ( viewMatrix * Vec4f( p_worldPos.x, p_worldPos.y, p_worldPos.z, 1.f ) );

		Vec3f screenPointPos;
		if ( screenPos.w != 0 )
		{
			const float absW		= screenPos.w < 0 ? -screenPos.w : screenPos.w;
			const Vec3f viewportPos = Vec3f( screenPos ) / absW;
			screenPointPos			= Vec3f( ( viewportPos.x + 1.f ) * 0.5f * p_renderRect.width(),
									 ( -viewportPos.y + 1.f ) * 0.5f * p_renderRect.height(),
									 viewportPos.z );
		}
		else
		{
			screenPointPos = { 0, 0, -1000 };
		}

		return screenPointPos;
	}

	Vec3f screenToWorldVector( const QPoint &							   p_screenPos,
							   const VTX::App::Component::Render::Camera & p_camera,
							   const QRect &							   p_renderRect )
	{
		const Vec3f	  worldRayStart = p_camera.getPosition();
		const QPointF normalizedPos = QPointF( ( 2 * ( p_screenPos.x() / float( p_renderRect.width() ) ) ) - 1,
											   1 - ( 2 * ( p_screenPos.y() / float( p_renderRect.height() ) ) ) );
		const Vec4f	  ray_clip		= Vec4f( normalizedPos.x(), normalizedPos.y(), -1.0, 1.0 );

		Vec4f ray_eye = Util::Math::inverse( p_camera.getProjectionMatrix() ) * ray_clip;
		ray_eye		  = Vec4f( ray_eye.x, ray_eye.y, -1.0, 0.0 );

		const Vec4f ray_world = Util::Math::inverse( p_camera.getViewMatrix() ) * ray_eye;
		return Util::Math::normalize( Vec3f( ray_world.x, ray_world.y, ray_world.z ) );
	}

	bool isInSphere( const Vec3f &								 p_worldSphereCenter,
					 const float								 p_sphereRadius,
					 const QPoint &								 p_screenPos,
					 const VTX::App::Component::Render::Camera & p_camera,
					 const QRect &								 p_renderRect )
	{
		const Vec3f pointRay		  = screenToWorldVector( p_screenPos, p_camera, p_renderRect );
		const Vec3f relativeSpherePos = p_camera.getPosition() - p_worldSphereCenter;

		const float b = Util::Math::dot( pointRay, relativeSpherePos );
		const float c = Util::Math::dot( relativeSpherePos, relativeSpherePos ) - ( p_sphereRadius * p_sphereRadius );

		return ( b * b - c ) >= 0;
	}

	void fillScreenPositions( const std::vector<Vec3f> &				  p_worldPositions,
							  std::vector<QPoint> &						  p_screenPositions,
							  const VTX::App::Component::Render::Camera & p_camera,
							  const QRect &								  p_renderRect )
	{
		p_screenPositions.resize( p_worldPositions.size() );

		for ( int i = 0; i < p_worldPositions.size(); i++ )
		{
			p_screenPositions[ i ] = worldToScreenQPoint( p_worldPositions[ i ], p_camera, p_renderRect );
		}
	}
	void fillScreenPositions( const std::vector<Vec3f> &				  p_worldPositions,
							  std::vector<Vec3f> &						  p_screenPositions,
							  const VTX::App::Component::Render::Camera & p_camera,
							  const QRect &								  p_renderRect )
	{
		p_screenPositions.resize( p_worldPositions.size() );

		for ( int i = 0; i < p_worldPositions.size(); i++ )
		{
			p_screenPositions[ i ] = worldToScreenVec3f( p_worldPositions[ i ], p_camera, p_renderRect );
		}
	}

	Vec3f getCenter( const std::vector<Vec3f> & p_worldPositions )
	{
		const float factor = 1 / float( p_worldPositions.size() );
		Vec3f		res	   = VEC3F_ZERO;

		for ( const Vec3f & pos : p_worldPositions )
			res += pos * factor;

		return res;
	}

	void getMinMax( const std::vector<QPoint> & p_positions, int & p_minX, int & p_maxX, int & p_minY, int & p_maxY )
	{
		if ( p_positions.size() == 0 )
		{
			p_minX = p_maxX = p_minY = p_maxY = 0;
		}
		else
		{
			p_minX = p_maxX = p_positions[ 0 ].x();
			p_minY = p_maxY = p_positions[ 0 ].y();
		}

		for ( int i = 1; i < p_positions.size(); i++ )
		{
			const QPoint & position = p_positions[ i ];

			p_minX = p_minX < position.x() ? p_minX : position.x();
			p_maxX = p_maxX > position.x() ? p_maxX : position.x();
			p_minY = p_minY < position.y() ? p_minY : position.y();
			p_maxY = p_maxY > position.y() ? p_maxY : position.y();
		}
	}

	bool isVisibleToCamera( const VTX::App::Component::Render::Camera & p_camera, const Vec3f & p_worldPos )
	{
		const Vec3f & cameraForward = p_camera.getFront();
		const Vec3f	  clipPos		= p_camera.getPosition() + cameraForward;
		const Vec3f	  cameraToPos	= Util::Math::normalize( p_worldPos - clipPos );

		return Util::Math::dot( cameraForward, cameraToPos ) > 0;
	}

	bool anyVisibleToCamera( const VTX::App::Component::Render::Camera & p_camera,
							 const std::vector<Vec3f> &					 p_worldPositions )
	{
		for ( const Vec3f & position : p_worldPositions )
		{
			if ( isVisibleToCamera( p_camera, position ) )
				return true;
		}

		return false;
	}

	float distanceToCamera( const VTX::App::Component::Render::Camera & p_camera, const Vec3f & p_worldPos )
	{
		return Util::Math::distance( p_camera.getPosition(), p_worldPos );
	}

	QLine getScreenLine( const Vec3f & p_point1, const Vec3f & p_point2 )
	{
		QPoint line1From;
		QPoint line1To;

		if ( p_point1.z >= 0 && p_point2.z >= 0 )
		{
			line1From = vec3fToQPoint( p_point1 );
			line1To	  = vec3fToQPoint( p_point2 );
		}
		else if ( p_point1.z >= 0 && p_point2.z < 0 || p_point1.z < 0 && p_point2.z >= 0 )
		{
			const Vec3f & inScreenPoint	 = p_point1.z >= 0 ? p_point1 : p_point2;
			const Vec3f & outScreenPoint = p_point1.z < 0 ? p_point1 : p_point2;

			const Vec3f vec2_1 = Vec3f( outScreenPoint - inScreenPoint );
			const Vec3f dir	   = Util::Math::normalize( Vec3f( outScreenPoint - inScreenPoint ) );

			const float ratioDisplayed = inScreenPoint.z / float( inScreenPoint.z - outScreenPoint.z );

			line1From = vec3fToQPoint( inScreenPoint );
			line1To	  = vec3fToQPoint( inScreenPoint + ( dir * ratioDisplayed * Util::Math::length( vec2_1 ) ) );
		}
		else
		{
			line1From = QPoint( 0, 0 );
			line1To	  = QPoint( 0, 0 );
		}

		return QLine( line1From, line1To );
	}

} // namespace VTX::Util::UIRender
