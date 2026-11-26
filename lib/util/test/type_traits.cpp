#include <catch2/catch_test_macros.hpp>
#include <util/type_traits.hpp>

TEST_CASE( "Util::type_traits", "[unit]" )
{
	using namespace VTX;
	const Vec2f				v2f {};
	const Vec2i				v2i {};
	const Vec3f				v3f {};
	const Vec3i				v3i {};
	const Util::Color::Rgba color {};

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
