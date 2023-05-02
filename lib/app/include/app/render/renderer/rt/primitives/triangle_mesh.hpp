#ifndef __VTX_APP_RENDER_RENDERER_RT_TRIANGLE_MESH__
#define __VTX_APP_RENDER_RENDERER_RT_TRIANGLE_MESH__

#include "../bvh.hpp"
#include "app/component/chemistry/molecule.hpp"
#include <util/constants.hpp>

namespace VTX::App::Render::Renderer::RT::Primitive
{
	class Triangle;

	class TriangleMesh : public BaseObject3D
	{
	  public:
		friend Triangle;
		TriangleMesh() = default;
		// explicit TriangleMesh( const FilePath & meshToLoad );
		// explicit TriangleMesh( const App::Component::Chemistry::Molecule * p_molecule );

		~TriangleMesh()
		{
			for ( Material::BaseMaterial * mtl : _materials )
			{
				delete mtl;
			}
			_materials.clear();
		}

		bool intersect( const Ray &	   p_ray,
						const float	   p_tMin,
						const float	   p_tMax,
						Intersection & p_intersection ) const override
		{
			return _bvh.intersect( p_ray, p_tMin, p_tMax, p_intersection );
		}

		bool intersectAny( const Ray & p_ray, const float p_tMin, const float p_tMax ) const override
		{
			return _bvh.intersectAny( p_ray, p_tMin, p_tMax );
		}

	  private:
		void _computeAABB() override
		{
			if ( _bvh.isBuilt() )
				_aabb = _bvh.getAABB();
			// TODO: else compute it !
		}

	  private:
		BVH									  _bvh;
		std::vector<BasePrimitive *>		  _triangles;
		std::vector<Vec3f>					  _vertices;
		std::vector<Vec3f>					  _normals;
		std::vector<Material::BaseMaterial *> _materials;
	};

	class Triangle : public BasePrimitive
	{
	  public:
		friend class TriangleMesh;

		Triangle() = default;
		Triangle( const uint p_v0, const uint p_v1, const uint p_v2, TriangleMesh * p_refMesh ) :
			_v0( p_v0 ), _v1( p_v1 ), _v2( p_v2 ), _refMesh( p_refMesh )
		{
		}

		// bool intersect( const Ray &	   p_ray,
		//				const float	   p_tMin,
		//				const float	   p_tMax,
		//				Intersection & p_intersection ) const
		//{
		//	// Moller-Trumbore
		//	const Vec3f & o	 = p_ray.getOrigin();
		//	const Vec3f & d	 = p_ray.getDirection();
		//	const Vec3f & v0 = _refMesh->_vertices[ _v0 ];
		//	const Vec3f & v1 = _refMesh->_vertices[ _v1 ];
		//	const Vec3f & v2 = _refMesh->_vertices[ _v2 ];

		//	const Vec3f edge1 = v1 - v0;
		//	const Vec3f edge2 = v2 - v0;

		//	const Vec3f pVec = Util::cross( d, edge2 );
		//	const float det	 = Util::dot( edge1, pVec );

		//	if ( det == 0.f ) return false;

		//	const float invDet = 1.f / det;

		//	const Vec3f tVec = o - v0;

		//	const float u = Util::dot( tVec, pVec ) * invDet;
		//	if ( u < 0.f || u > 1.f ) return false;

		//	const Vec3f qVec = Util::cross( tVec, edge1 );
		//	const float v	 = Util::dot( d, qVec ) * invDet;
		//	const float w	 = 1.f - u - v;
		//	if ( v < 0.f || w < 0.f ) return false;

		//	const float t = Util::dot( edge2, qVec ) * invDet;

		//	if ( t < p_tMin || t > p_tMax ) { return false; }

		//	p_intersection._point	  = p_ray.getPointAtT( t );
		//	const Vec3f normal		  = _interpolateNormal( u, v, w );
		//	p_intersection._normal	  = Util::faceForward( normal, d );
		//	p_intersection._distance  = t;
		//	p_intersection._primitive = this;

		//	return true;
		//}

		bool intersect( const Ray & p_ray, const float p_tMin, const float p_tMax, Intersection & p_intersection ) const
		{
			const Vec3f & o	 = p_ray.getOrigin();
			const Vec3f & rd = p_ray.getDirection();
			const Vec3f & v0 = _refMesh->_vertices[ _v0 ];
			const Vec3f & v1 = _refMesh->_vertices[ _v1 ];
			const Vec3f & v2 = _refMesh->_vertices[ _v2 ];

			// Translate vertices based on ray origin
			Vec3f p0t = v0 - o;
			Vec3f p1t = v1 - o;
			Vec3f p2t = v2 - o;

			// Permute components of triangle vertices and ray direction
			int kz = ( fabsf( rd.x ) > fabsf( rd.y ) ) ? ( ( fabsf( rd.x ) > fabsf( rd.z ) ) ? 0 : 2 )
													   : ( ( fabsf( rd.y ) > fabsf( rd.z ) ) ? 1 : 2 );
			int kx = kz + 1;
			if ( kx == 3 )
				kx = 0;
			int ky = kx + 1;
			if ( ky == 3 )
				ky = 0;
			Vec3f d = _permute( rd, kx, ky, kz );
			p0t		= _permute( p0t, kx, ky, kz );
			p1t		= _permute( p1t, kx, ky, kz );
			p2t		= _permute( p2t, kx, ky, kz );

			// Apply shear transformation to translated vertex positions
			float Sx = -d.x / d.z;
			float Sy = -d.y / d.z;
			float Sz = 1.f / d.z;
			p0t.x += Sx * p0t.z;
			p0t.y += Sy * p0t.z;
			p1t.x += Sx * p1t.z;
			p1t.y += Sy * p1t.z;
			p2t.x += Sx * p2t.z;
			p2t.y += Sy * p2t.z;

			// Compute edge function coefficients e0, e1, and e2
			float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
			float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
			float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

			// Fall back to double-precision test at triangle edges
			if ( ( e0 == 0.0f || e1 == 0.0f || e2 == 0.0f ) )
			{
				double p2txp1ty = (double)p2t.x * (double)p1t.y;
				double p2typ1tx = (double)p2t.y * (double)p1t.x;
				e0				= (float)( p2typ1tx - p2txp1ty );
				double p0txp2ty = (double)p0t.x * (double)p2t.y;
				double p0typ2tx = (double)p0t.y * (double)p2t.x;
				e1				= (float)( p0typ2tx - p0txp2ty );
				double p1txp0ty = (double)p1t.x * (double)p0t.y;
				double p1typ0tx = (double)p1t.y * (double)p0t.x;
				e2				= (float)( p1typ0tx - p1txp0ty );
			}

			// Perform triangle edge and determinant tests
			if ( ( e0 < 0.f || e1 < 0.f || e2 < 0.f ) && ( e0 > 0.f || e1 > 0.f || e2 > 0.f ) )
				return false;
			float det = e0 + e1 + e2;
			if ( det == 0.f )
				return false;

			// Compute scaled hit distance to triangle and test against ray  range
			p0t.z *= Sz;
			p1t.z *= Sz;
			p2t.z *= Sz;
			float tScaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
			if ( det < 0.f && ( tScaled >= 0.f || tScaled < p_tMax * det ) )
				return false;
			else if ( det > 0.f && ( tScaled <= 0.f || tScaled > p_tMax * det ) )
				return false;

			// Compute barycentric coordinates and  value for triangle intersection
			float invDet = 1 / det;
			float b0	 = e0 * invDet;
			float b1	 = e1 * invDet;
			float b2	 = e2 * invDet;
			float t		 = tScaled * invDet;

			// Ensure that computed triangle $t$ is conservatively greater than zero

			// Compute $\delta_z$ term for triangle $t$ error bounds
			float maxZt	 = Util::Math::max( fabsf( p0t.z ), Util::Math::max( fabsf( p1t.z ), fabsf( p2t.z ) ) );
			float deltaZ = _gamma( 3 ) * maxZt;

			// Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
			float maxXt	 = Util::Math::max( fabsf( p0t.x ), Util::Math::max( fabsf( p1t.x ), fabsf( p2t.x ) ) );
			float maxYt	 = Util::Math::max( fabsf( p0t.y ), Util::Math::max( fabsf( p1t.y ), fabsf( p2t.y ) ) );
			float deltaX = _gamma( 5 ) * ( maxXt + maxZt );
			float deltaY = _gamma( 5 ) * ( maxYt + maxZt );

			// Compute $\delta_e$ term for triangle $t$ error bounds
			float deltaE = 2.f * ( _gamma( 2 ) * maxXt * maxYt + deltaY * maxXt + deltaX * maxYt );

			// Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
			float maxE	 = Util::Math::max( fabsf( e0 ), Util::Math::max( fabsf( e1 ), fabsf( e2 ) ) );
			float deltaT = 3.f * ( _gamma( 3 ) * maxE * maxZt + deltaE * maxZt + deltaZ * maxE ) * std::abs( invDet );
			if ( t <= deltaT )
				return false;

			p_intersection._point = b0 * v0 + b1 * v1 + b2 * v2;
			// p_ray.getPointAtT( t );
			const Vec3f normal
				= b0 * _refMesh->_normals[ _v0 ] + b1 * _refMesh->_normals[ _v1 ] + b2 * _refMesh->_normals[ _v2 ];
			;
			p_intersection._normal	  = Util::Math::faceForward( normal, rd );
			p_intersection._distance  = t;
			p_intersection._primitive = this;

			return true;
		}

	  private:
		inline float _gamma( int n ) const { return ( n * FLT_EPSILON ) / ( 1.f - n * FLT_EPSILON ); }
		Vec3f		 _permute( const Vec3f & p, int x, int y, int z ) const { return Vec3f( p[ x ], p[ y ], p[ z ] ); }
		Vec3f		 _interpolateNormal( const float p_u, const float p_v, const float p_w ) const
		{
			// should not be necessary to normalize, but for safety...
			return Util::Math::normalize( _refMesh->_normals[ _v0 ] * p_w + _refMesh->_normals[ _v1 ] * p_u
										  + _refMesh->_normals[ _v2 ] * p_v );
		}

		void _computeAABB()
		{
			_aabb.extend( _refMesh->_vertices[ _v0 ] );
			_aabb.extend( _refMesh->_vertices[ _v1 ] );
			_aabb.extend( _refMesh->_vertices[ _v2 ] );
		}

	  private:
		union
		{
			struct
			{
				uint _v0, _v1, _v2;
			};
			uint _v[ 3 ] = { INVALID_ID, INVALID_ID, INVALID_ID };
		};
		TriangleMesh * _refMesh = nullptr;
	};

} // namespace VTX::App::Render::Renderer::RT::Primitive

#endif
