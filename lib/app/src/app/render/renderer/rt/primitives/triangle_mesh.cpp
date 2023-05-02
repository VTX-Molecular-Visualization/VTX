#include "app/render/renderer/rt/primitives/triangle_mesh.hpp"
#include "app/render/renderer/rt/materials/flat_color_material.hpp"
#include "app/render/renderer/rt/materials/matte.hpp"
#include "app/render/renderer/rt/materials/phong_material.hpp"
#include <iostream>

namespace VTX::App::Render::Renderer::RT::Primitive
{
	/*
	TriangleMesh::TriangleMesh( const FilePath & meshToLoad )
	{
		Assimp::Importer importer;

		// Read scene and triangulate meshes
		const aiScene * const scene
			= importer.ReadFile( meshToLoad.string(), aiProcess_Triangulate | aiProcess_GenNormals );

		if ( scene == nullptr )
		{
			throw std::runtime_error( "Cannot import file: " + meshToLoad.string() );
		}

		const uint nbMeshes	   = scene->mNumMeshes;
		uint	   nbTriangles = 0;
		uint	   nbVertices  = 0;
		_materials.emplace_back( new MatteMaterial( Color::Rgb( 0.8f, 0.f, 0.f ) ) );

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
	*/

	/*
	TriangleMesh::TriangleMesh( const App::Component::Chemistry::Molecule * p_molecule )
	{
		const Model::Ribbon & ribbon	  = p_molecule->getRibbon();
		_vertices						  = ribbon.getVertices();
		_normals						  = ribbon.getNormals();
		const std::vector<uint> & indices = ribbon.getIndices();

		_triangles.reserve( indices.size() / 3 );

		for ( uint i = 0; i < uint( indices.size() ); i += 3 )
		{
			uint idMtl = INVALID_ID;
			for ( uint m = 0; m < uint( _materials.size() ); ++m )
			{
				if ( _materials[ m ]->getColor() == ribbon.getColor( indices[ i ] ) )
				{
					idMtl = m;
					break;
				}
			}
			if ( idMtl == INVALID_ID )
			{
				_materials.emplace_back( //
										 // new MatteMaterial( colors[ indices[ i ] ], 0.3f ) );
					new PhongMaterial( ribbon.getColor( indices[ i ] ), ribbon.getColor( indices[ i ] ), 64.f ) );
				idMtl = uint( _materials.size() - 1 );
			}

			_triangles.emplace_back( new Triangle( indices[ i ], indices[ i + 1 ], indices[ i + 2 ], this ) );
			Triangle & tri = *( (Triangle *)_triangles.back() );
			tri._material  = _materials[ idMtl ];
		}

		// compute AABB for each triangle
		for ( uint i = 0; i < uint( _triangles.size() ); ++i )
		{
			Triangle & tri = *( (Triangle *)_triangles[ i ] );
			tri._computeAABB();
		}

		const uint maxPrimsLeaf = 256;
		_bvh.build( _triangles, maxPrimsLeaf, BVH::SplitMethod::SAH );
	}
	*/
} // namespace VTX::App::Render::Renderer::RT::Primitive
