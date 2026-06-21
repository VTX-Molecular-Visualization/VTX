#include <app/action/io.hpp>
#include <app/fixture.hpp>
#include <app/generic/name.hpp>
#include <app/services.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/mesh.hpp>
#include <iterator>
#include <util/filesystem.hpp>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>

TEST_CASE( "VTX_APP - Mesh - Load", "[mesh][load]" )
{
	using namespace VTX;
	App::Fixture app;

	const FilePath meshPath = Util::Filesystem::getExecutableDir() / "data" / "two_triangles.obj";
	App::ACTION().execute<App::Action::IO::Open>( meshPath );

	auto meshes = App::REG().view<Core::Struct::Mesh, App::Generic::Name, Util::Math::AABB, Util::Math::Transform>();
	REQUIRE( std::distance( meshes.begin(), meshes.end() ) == 1 );

	const Entity entity = *meshes.begin();
	const auto & mesh	= meshes.get<Core::Struct::Mesh>( entity );
	const auto & name	= meshes.get<App::Generic::Name>( entity );
	const auto & aabb	= meshes.get<Util::Math::AABB>( entity );
	CHECK( mesh.vertices.size() == 6 );
	CHECK( mesh.indices.size() == 6 );
	CHECK( name.name == "two_triangles" );
	CHECK( aabb.getMin() == Vec3f( 0.f, 0.f, 0.f ) );
	CHECK( aabb.getMax() == Vec3f( 3.f, 1.f, 0.f ) );
}
