#include <catch2/catch_test_macros.hpp>
#include <util/type_traits.hpp>

TEST_CASE( "Util::type_traits", "[unit]" )
{
	using namespace VTX;
	Vec2f			  v2f;
	Vec2i			  v2i;
	Vec3f			  v3f;
	Vec3i			  v3i;
	Util::Color::Rgba color;

	CHECK( is_vec2f_v<decltype( v2f )> );
	CHECK( !is_vec2f_v<decltype( v2i )> );
	CHECK( !is_vec2f_v<decltype( v3f )> );
	CHECK( !is_vec2f_v<decltype( v3i )> );
	CHECK( !is_vec2f_v<decltype( color )> );
	CHECK( !is_vec2i_v<decltype( v2f )> );
	CHECK( is_vec2i_v<decltype( v2i )> );
	CHECK( !is_vec2i_v<decltype( v3f )> );
	CHECK( !is_vec2i_v<decltype( v3i )> );
	CHECK( !is_vec2i_v<decltype( color )> );
	CHECK( !is_vec3f_v<decltype( v2f )> );
	CHECK( !is_vec3f_v<decltype( v2i )> );
	CHECK( is_vec3f_v<decltype( v3f )> );
	CHECK( !is_vec3f_v<decltype( v3i )> );
	CHECK( !is_vec3f_v<decltype( color )> );
	CHECK( !is_vec3i_v<decltype( v2f )> );
	CHECK( !is_vec3i_v<decltype( v2i )> );
	CHECK( !is_vec3i_v<decltype( v3f )> );
	CHECK( is_vec3i_v<decltype( v3i )> );
	CHECK( !is_vec3i_v<decltype( color )> );
	CHECK( !is_color4_v<decltype( v2f )> );
	CHECK( !is_color4_v<decltype( v2i )> );
	CHECK( !is_color4_v<decltype( v3f )> );
	CHECK( !is_color4_v<decltype( v3i )> );
	CHECK( is_color4_v<decltype( color )> );
}
