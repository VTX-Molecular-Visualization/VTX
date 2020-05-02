#ifndef __VTX_RENDERER_RAY_TRACER_TRIANGLE_MESH__
#define __VTX_RENDERER_RAY_TRACER_TRIANGLE_MESH__

#ifdef _MSC_VER
#pragma once
#endif

#include "../bvh.hpp"
#include "io/path.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace Renderer
	{
		class Triangle;

		class TriangleMesh : public BaseObject3D
		{
		  public:
			friend Triangle;
			TriangleMesh() = default;
			explicit TriangleMesh( const IO::Path & meshToLoad );
			explicit TriangleMesh( const Model::Molecule * p_molecule );

			~TriangleMesh()
			{
				for ( Renderer::BaseMaterial * mtl : _materials )
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
				if ( _bvh.isBuilt() ) _aabb = _bvh.getAABB();
				// TODO: else compute it !
			}

		  private:
			BVH									  _bvh;
			std::vector<BasePrimitive *>		  _triangles;
			std::vector<Vec3f>					  _vertices;
			std::vector<Vec3f>					  _normals;
			std::vector<Renderer::BaseMaterial *> _materials;
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

			bool intersect( const Ray &	   p_ray,
							const float	   p_tMin,
							const float	   p_tMax,
							Intersection & p_intersection ) const
			{
				// Moller-Trumbore
				const Vec3f & o	 = p_ray.getOrigin();
				const Vec3f & d	 = p_ray.getDirection();
				const Vec3f & v0 = _refMesh->_vertices[ _v0 ];
				const Vec3f & v1 = _refMesh->_vertices[ _v1 ];
				const Vec3f & v2 = _refMesh->_vertices[ _v2 ];

				const Vec3f edge1 = v1 - v0;
				const Vec3f edge2 = v2 - v0;

				const Vec3f pVec = Util::Math::cross( d, edge2 );
				const float det	 = Util::Math::dot( edge1, pVec );

				if ( det == 0.f ) return false;

				const float invDet = 1.f / det;

				const Vec3f tVec = o - v0;

				const float u = Util::Math::dot( tVec, pVec ) * invDet;
				if ( u < 0.f || u > 1.f ) return false;

				const Vec3f qVec = Util::Math::cross( tVec, edge1 );
				const float v	 = Util::Math::dot( d, qVec ) * invDet;
				const float w	 = 1.f - u - v;
				if ( v < 0.f || w < 0.f ) return false;

				const float t = Util::Math::dot( edge2, qVec ) * invDet;

				if ( t < p_tMin || t > p_tMax ) { return false; }

				p_intersection._point	  = p_ray.getPointAtT( t );
				const Vec3f normal		  = _interpolateNormal( u, v, w );
				p_intersection._normal	  = Util::Math::faceForward( normal, d );
				p_intersection._distance  = t;
				p_intersection._primitive = this;

				return true;
			}

		  private:
			Vec3f _interpolateNormal( const float p_u, const float p_v, const float p_w ) const
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

	} // namespace Renderer
} // namespace VTX

#endif
