#include "triangle_mesh.hpp"
#include "../materials/diffuse_material.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

namespace VTX
{
	namespace Renderer
	{
		TriangleMesh::TriangleMesh( const IO::Path & meshToLoad )
		{
			Assimp::Importer importer;

			// Read scene and triangulate meshes
			const aiScene * const scene = importer.ReadFile( meshToLoad.c_str(), aiProcess_Triangulate );

			if ( scene == nullptr ) { throw std::runtime_error( "Cannot import file: " + meshToLoad.str() ); }

			const uint nbMeshes	   = scene->mNumMeshes;
			uint	   nbTriangles = 0;
			uint	   nbVertices  = 0;
			_materials.emplace_back( new DiffuseMaterial( Vec3f( 0.8f, 0.f, 0.f ) ) );

			for ( uint i = 0; i < nbMeshes; ++i )
			{
				const aiMesh * const mesh = scene->mMeshes[ i ];
				nbTriangles += mesh->mNumFaces;
				nbVertices += mesh->mNumVertices;
			}

			_triangles.resize( nbTriangles );
			_vertices.resize( nbVertices );

			uint currentTriangle = 0;
			uint currentVertex	 = 0;

			for ( uint m = 0; m < nbMeshes; ++m )
			{
				const aiMesh * const	 mesh	  = scene->mMeshes[ m ];
				const aiMaterial * const material = scene->mMaterials[ mesh->mMaterialIndex ];

				for ( uint f = 0; f < mesh->mNumFaces; ++f, ++currentTriangle )
				{
					const aiFace face = mesh->mFaces[ f ];

					_triangles[ currentTriangle ] = new Triangle();
					Triangle & tri				  = *( (Triangle *)_triangles[ currentTriangle ] );
					tri._refMesh				  = this;
					// triangulated ! :-)
					for ( uint v = 0; v < 3; ++v )
					{
						const uint idV = face.mIndices[ v ];
						tri._v[ v ]	   = idV;
					}
					tri._material = _materials.back();
				}

				for ( uint v = 0; v < mesh->mNumVertices; ++v, ++currentVertex )
				{
					Vec3f & vertex = _vertices[ currentVertex ];
					vertex.x	   = mesh->mVertices[ v ].x;
					vertex.y	   = mesh->mVertices[ v ].y;
					vertex.z	   = mesh->mVertices[ v ].z;
				}
			}
			// compute AABB for each triangle
			for ( uint i = 0; i < uint( _triangles.size() ); ++i )
			{
				Triangle & tri = *( (Triangle *)_triangles[ i ] );
				tri._computeAABB();
			}

			std::cout << "Loaded: " << std::endl
					  << "- nb faces: " << _triangles.size() << std::endl
					  << "- nb vertices: " << _vertices.size() << std::endl;

			const uint maxPrimsLeaf = 8;
			_bvh.build( _triangles, maxPrimsLeaf, BVH::SplitMethod::SAH );
			const Math::AABB & aabb2	 = _bvh.getAABB();
			const Vec3f		   centroid2 = aabb2.centroid();
			std::cout << "centroid2 " << centroid2.x << " - " << centroid2.y << " - " << centroid2.z << std::endl;
			std::cout << "min2 " << aabb2._min.x << " - " << aabb2._min.y << " - " << aabb2._min.z << std::endl;
			std::cout << "min2 " << aabb2._max.x << " - " << aabb2._max.y << " - " << aabb2._max.z << std::endl;
		}
	} // namespace Renderer
} // namespace VTX
