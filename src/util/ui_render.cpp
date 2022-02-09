#include "util/ui_render.hpp"
#include "object3d/camera.hpp"

namespace VTX::Util::UIRender
{
	QPoint worldToScreen( const Vec3f & p_worldPos, const Object3D::Camera & p_camera, const QRect & p_renderRect )
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

	void fillScreenPositions( const std::vector<Vec3f> & p_worldPositions,
							  std::vector<QPoint> &		 p_screenPositions,
							  const Object3D::Camera &	 p_camera,
							  const QRect &				 p_renderRect )
	{
		p_screenPositions.resize( p_worldPositions.size() );

		for ( int i = 0; i < p_worldPositions.size(); i++ )
		{
			p_screenPositions[ i ] = worldToScreen( p_worldPositions[ i ], p_camera, p_renderRect );
		}
	}

	Vec3f getCenter( const std::vector<Vec3f> & p_worldPositions )
	{
		const float factor = 1 / float(p_worldPositions.size());
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

	bool isVisibleToCamera( const Object3D::Camera & p_camera, const Vec3f & p_worldPos )
	{
		const Vec3f & cameraForward = p_camera.getFront();
		const Vec3f	  clipPos		= p_camera.getPosition() + cameraForward;
		const Vec3f	  cameraToPos	= Util::Math::normalize( p_worldPos - clipPos );

		return Util::Math::dot( cameraForward, cameraToPos ) > 0;
	}

	bool anyVisibleToCamera( const Object3D::Camera & p_camera, const std::vector<Vec3f> & p_worldPositions )
	{
		for ( const Vec3f & position : p_worldPositions )
		{
			if ( isVisibleToCamera( p_camera, position ) )
				return true;
		}

		return false;
	}

	float distanceToCamera( const Object3D::Camera & p_camera, const Vec3f & p_worldPos )
	{
		return Util::Math::distance( p_camera.getPosition(), p_worldPos );
	}

} // namespace VTX::Util::UIRender
