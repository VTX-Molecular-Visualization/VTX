#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/mesh.hpp>
#include <io/mesh_reader.hpp>
#include <util/filesystem.hpp>
#include <util/thread.hpp>

TEST_CASE( "VTX_IO - Read OBJ mesh", "[reader][mesh]" )
{
	using namespace VTX;

	const FilePath meshPath = Util::Filesystem::getExecutableDir() / "data" / "two_triangles.obj";

	std::vector<Core::Struct::Mesh> meshes;
	Util::StopToken					stopToken;
	IO::MeshReader					reader( meshPath, stopToken );
	reader.get( meshes );

	REQUIRE( not meshes.empty() );
	size_t vertexCount = 0;
	size_t indexCount  = 0;

	for ( const Core::Struct::Mesh & mesh : meshes )
	{
		CHECK( mesh.normals.size() == mesh.vertices.size() );
		for ( const Index index : mesh.indices )
		{
			CHECK( index < mesh.vertices.size() );
		}
		vertexCount += mesh.vertices.size();
		indexCount += mesh.indices.size();
	}

	CHECK( vertexCount == 6 );
	CHECK( indexCount == 6 );
}

TEST_CASE( "VTX_IO - Detect Assimp mesh formats", "[reader][mesh]" )
{
	CHECK( VTX::IO::isMeshFileFormat( "mesh.obj" ) );
	CHECK( VTX::IO::isMeshFileFormat( "mesh.fbx" ) );
	CHECK( VTX::IO::isMeshFileFormat( "mesh.gltf" ) );
	CHECK( VTX::IO::isMeshFileFormat( "mesh.glb" ) );
	CHECK( not VTX::IO::isMeshFileFormat( "mesh.unknown" ) );
}
