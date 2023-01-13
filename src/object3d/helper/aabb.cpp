#include "aabb.hpp"
#include "object3d/camera.hpp"
#include "util/math.hpp"

namespace VTX::Object3D::Helper
{

	AABB::AABB( const Vec3f & p_min, const Vec3f & p_max ) : data { { p_min, p_max } } {}
	AABB::AABB( const Vec3f & p_center, const float p_radius ) : data { { p_center - p_radius, p_center + p_radius } }
	{
	}

	void AABB::extend( const Vec3f & p_point )
	{
		data.minMax.min = Util::Math::min( p_point, data.minMax.min );
		data.minMax.max = Util::Math::max( p_point, data.minMax.max );
	}

	void AABB::extend( const AABB & p_aabb )
	{
		data.minMax.min = Util::Math::min( data.minMax.min, p_aabb.data.minMax.min );
		data.minMax.max = Util::Math::max( data.minMax.max, p_aabb.data.minMax.max );
	}

	void AABB::extend( const Vec3f & p_center, const float p_radius )
	{
		extend( p_center - p_radius );
		extend( p_center + p_radius );
	}

	void AABB::translate( const Vec3f & p_translation )
	{
		data.minMax.min += p_translation;
		data.minMax.max += p_translation;
	}

	bool AABB::intersect( const Vec3f & p_rayPosition,
						  const Vec3f & p_rayInvDir,
						  const Vec3i & p_isDirNeg,
						  const float	p_tMin,
						  const float	p_tMax ) const
	{
		float tMin = ( data.limits[ p_isDirNeg.x ].x - p_rayPosition.x ) * p_rayInvDir.x;
		float tMax = ( data.limits[ 1 - p_isDirNeg.x ].x - p_rayPosition.x ) * p_rayInvDir.x;

		const float tYMin = ( data.limits[ p_isDirNeg.y ].y - p_rayPosition.y ) * p_rayInvDir.y;
		const float tYMax = ( data.limits[ 1 - p_isDirNeg.y ].y - p_rayPosition.y ) * p_rayInvDir.y;

		if ( tMin > tYMax || tYMin > tMax )
			return false;
		if ( tYMin > tMin )
			tMin = tYMin;
		if ( tYMax < tMax )
			tMax = tYMax;

		const float tZmin = ( data.limits[ p_isDirNeg.z ].z - p_rayPosition.z ) * p_rayInvDir.z;
		const float tZmax = ( data.limits[ 1 - p_isDirNeg.z ].z - p_rayPosition.z ) * p_rayInvDir.z;

		if ( tMin > tZmax || tZmin > tMax )
			return false;
		if ( tZmin > tMin )
			tMin = tZmin;
		if ( tZmax < tMax )
			tMax = tZmax;

		return ( ( tMin < p_tMax ) && ( tMax > p_tMin ) );
	}

	AABB AABB::join( const AABB & p_aabb1, const AABB & p_aabb2 )
	{
		return AABB( Util::Math::min( p_aabb1.data.minMax.min, p_aabb2.data.minMax.min ),
					 Util::Math::max( p_aabb1.data.minMax.max, p_aabb2.data.minMax.max ) );
	}

	void AABB::refresh()
	{
		const Vec3f &	   min	   = getMin();
		const Vec3f &	   max	   = getMax();
		std::vector<Vec3f> corners = std::vector<Vec3f>( { min,
														   Vec3f( max.x, min.y, min.z ),
														   Vec3f( max.x, max.y, min.z ),
														   Vec3f( min.x, max.y, min.z ),
														   Vec3f( min.x, min.y, max.z ),
														   Vec3f( max.x, min.y, max.z ),
														   max,
														   Vec3f( min.x, max.y, max.z ) } );

		std::vector<uint> indices
			= std::vector<uint>( { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7 } );

		_vbo.set<Vec3f>( corners );
		_ibo.set<uint>( indices );
		_iboSize = uint( indices.size() );
	}

	void AABB::_generate()
	{
		_program = VTX_PROGRAM_MANAGER().createProgram(
			"Line", { IO::FilePath( "line/line.vert" ), IO::FilePath( "line/line.frag" ) } );
		assert( _program != nullptr );

		_vbo.create();
		_ibo.create();

		_vao.create();

		_vao.bindElementBuffer( _ibo );

		_vao.enableAttribute( ATTRIBUTE_LOCATION::AABB_CORNER );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::AABB_CORNER, _vbo, sizeof( Vec3f ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::AABB_CORNER, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::AABB_CORNER, ATTRIBUTE_LOCATION::AABB_CORNER );

		refresh();
	}

	void AABB::render( const Object3D::Camera & p_camera ) const
	{
		_program->use();

		const Mat4f MVMatrix = p_camera.getViewMatrix();
		_program->setMat4f( "u_MVMatrix", MVMatrix );
		_program->setMat4f( "u_projMatrix", p_camera.getProjectionMatrix() );
		_program->setMat4f( "u_normalMatrix", Util::Math::transpose( Util::Math::inverse( MVMatrix ) ) );

		_vao.bind();
		_vao.drawElement( Renderer::GL::VertexArray::DrawMode::LINES,
						  GLsizei( _iboSize ),
						  Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.unbind();
	}
} // namespace VTX::Object3D::Helper
