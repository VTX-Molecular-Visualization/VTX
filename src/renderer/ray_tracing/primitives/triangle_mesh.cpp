#include "triangle_mesh.hpp"
#include "../materials/matte.hpp"
#include "model/ribbon.hpp"
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
			const aiScene * const scene
				= importer.ReadFile( meshToLoad.c_str(), aiProcess_Triangulate | aiProcess_GenNormals );

			if ( scene == nullptr ) { throw std::runtime_error( "Cannot import file: " + meshToLoad.str() ); }

			const uint nbMeshes	   = scene->mNumMeshes;
			uint	   nbTriangles = 0;
			uint	   nbVertices  = 0;
			_materials.emplace_back( new MatteMaterial( Vec3f( 0.8f, 0.f, 0.f ) ) );

			for ( uint i = 0; i < nbMeshes; ++i )
			{
				const aiMesh * const mesh = scene->mMeshes[ i ];
				nbTriangles += mesh->mNumFaces;
				nbVertices += mesh->mNumVertices;
			}

			_triangles.resize( nbTriangles );
			_vertices.resize( nbVertices );
			_normals.resize( nbVertices );

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

					Vec3f & normal = _normals[ currentVertex ];
					normal.x	   = mesh->mNormals[ v ].x;
					normal.y	   = mesh->mNormals[ v ].y;
					normal.z	   = mesh->mNormals[ v ].z;
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
		}

		TriangleMesh::TriangleMesh( const Model::Molecule * p_molecule )
		{
			const Model::Ribbon & ribbon	   = p_molecule->getRibbon();
			_vertices						   = ribbon.getVertices();
			_normals						   = ribbon.getNormals();
			const std::vector<Vec3f> & colors  = ribbon.getColors();
			const std::vector<uint> &  indices = ribbon.getIndices();

			_materials.emplace_back( new MatteMaterial( Vec3f( 0.8f, 0.f, 0.f ) ) );

			_triangles.reserve( indices.size() / 3 );
			// TODO: once again, do not duplicate materials !
			for ( uint i = 0; i < uint( indices.size() ); i += 3 )
			{
				// const uint idTri	= i / 3;
				_triangles.emplace_back( new Triangle( indices[ i ], indices[ i + 1 ], indices[ i + 2 ], this ) );
				Triangle & tri = *( (Triangle *)_triangles.back() );
				tri._material  = _materials.back();
			}

			// compute AABB for each triangle
			for ( uint i = 0; i < uint( _triangles.size() ); ++i )
			{
				Triangle & tri = *( (Triangle *)_triangles[ i ] );
				tri._computeAABB();
			}

			const uint maxPrimsLeaf = 8;
			_bvh.build( _triangles, maxPrimsLeaf, BVH::SplitMethod::SAH );
		}
	} // namespace Renderer
} // namespace VTX
