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

	Core::Struct::Mesh mesh;
	Util::StopToken	   stopToken;
	IO::MeshReader	   reader( meshPath, stopToken );
	reader.get( mesh );

	CHECK( mesh.vertices.size() == 6 );
	CHECK( mesh.normals.size() == mesh.vertices.size() );
	CHECK( mesh.indices.size() == 6 );

	Index maxIndex = 0;
	for ( const Index index : mesh.indices )
	{
		CHECK( index < mesh.vertices.size() );
		maxIndex = std::max( maxIndex, index );
	}
	CHECK( maxIndex >= 3 );
}

TEST_CASE( "VTX_IO - Detect Assimp mesh formats", "[reader][mesh]" )
{
	CHECK( VTX::IO::isMeshFileFormat( "mesh.obj" ) );
	CHECK( VTX::IO::isMeshFileFormat( "mesh.fbx" ) );
	CHECK( VTX::IO::isMeshFileFormat( "mesh.gltf" ) );
	CHECK( VTX::IO::isMeshFileFormat( "mesh.glb" ) );
	CHECK( not VTX::IO::isMeshFileFormat( "mesh.unknown" ) );
}
