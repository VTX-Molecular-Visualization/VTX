#include "io/mesh_reader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <core/struct/mesh.hpp>
#include <util/constants.hpp>
#include <util/exceptions.hpp>
#include <util/thread.hpp>
#include <utility>

namespace VTX::IO
{
	namespace
	{
		constexpr unsigned int IMPORT_FLAGS = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices
											  | aiProcess_GenSmoothNormals | aiProcess_PreTransformVertices;
	}

	MeshReader::MeshReader( FilePath p_filePath, VTX::Util::StopToken & p_stopToken ) :
		_filePath( std::move( p_filePath ) ), _stopToken( p_stopToken )
	{
	}

	void MeshReader::get( VTX::Core::Struct::Mesh & p_mesh )
	{
		if ( not isMeshFileFormat( _filePath ) )
		{
			throw IOException( "Unsupported mesh format: {}", _filePath.extension().string() );
		}

		if ( _stopToken.get().stop_requested() )
		{
			return;
		}

		Assimp::Importer	  importer;
		const aiScene * const scene = importer.ReadFile( _filePath.string(), IMPORT_FLAGS );
		if ( scene == nullptr || not scene->HasMeshes() )
		{
			throw IOException( "Cannot load mesh '{}': {}", _filePath.string(), importer.GetErrorString() );
		}

		Core::Struct::Mesh mesh;
		size_t			   vertexCount = 0;
		size_t			   indexCount  = 0;
		for ( unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex )
		{
			const aiMesh & sourceMesh = *scene->mMeshes[ meshIndex ];
			vertexCount += sourceMesh.mNumVertices;
			indexCount += static_cast<size_t>( sourceMesh.mNumFaces ) * 3;
		}

		if ( vertexCount > TypeMax<Index> )
		{
			throw IOException( "Mesh '{}' has too many vertices", _filePath.string() );
		}

		mesh.vertices.reserve( vertexCount );
		mesh.normals.reserve( vertexCount );
		mesh.indices.reserve( indexCount );
		bool hasMissingNormals = false;

		for ( unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex )
		{
			if ( _stopToken.get().stop_requested() )
			{
				return;
			}

			const aiMesh & sourceMesh	= *scene->mMeshes[ meshIndex ];
			const Index	   vertexOffset = static_cast<Index>( mesh.vertices.size() );

			for ( unsigned int vertexIndex = 0; vertexIndex < sourceMesh.mNumVertices; ++vertexIndex )
			{
				const aiVector3D & vertex = sourceMesh.mVertices[ vertexIndex ];
				mesh.vertices.emplace_back( vertex.x, vertex.y, vertex.z );
				if ( sourceMesh.HasNormals() )
				{
					const aiVector3D & normal = sourceMesh.mNormals[ vertexIndex ];
					mesh.normals.emplace_back( normal.x, normal.y, normal.z );
				}
				else
				{
					mesh.normals.emplace_back( 0.f );
					hasMissingNormals = true;
				}
			}

			for ( unsigned int faceIndex = 0; faceIndex < sourceMesh.mNumFaces; ++faceIndex )
			{
				const aiFace & face = sourceMesh.mFaces[ faceIndex ];
				if ( face.mNumIndices != 3 )
				{
					continue;
				}

				for ( unsigned int index = 0; index < face.mNumIndices; ++index )
				{
					mesh.indices.emplace_back( vertexOffset + static_cast<Index>( face.mIndices[ index ] ) );
				}
			}
		}

		if ( mesh.indices.empty() )
		{
			throw IOException( "Mesh '{}' contains no triangles", _filePath.string() );
		}
		if ( hasMissingNormals )
		{
			mesh.computeNormals();
		}

		p_mesh = std::move( mesh );
	}

	bool isMeshFileFormat( const FilePath & p_path )
	{
		Assimp::Importer importer;
		return importer.IsExtensionSupported( p_path.extension().string() );
	}
} // namespace VTX::IO
