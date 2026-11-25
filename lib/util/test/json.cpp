#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <util/color/rgba.hpp>
#include <util/enum.hpp>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <util/json.hpp>
#include <util/types.hpp>

namespace VTX::Test::Json
{
	enum class E_JSON_TEST
	{
		ALPHA,
		BETA
	};

	enum class E_JSON_TEST_BITMASK : uint8_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		NONE  = 0,
		RED	  = 1u << 0,
		GREEN = 1u << 1,
		BLUE  = 1u << 2,
	};
} // namespace VTX::Test::Json

using VTX::Test::Json::E_JSON_TEST;
using VTX::Test::Json::E_JSON_TEST_BITMASK;

TEST_CASE( "Util::Json::Glm", "[unit]" )
{
	using VTX::Util::Json::Document;

	const VTX::Vec3f vec3 { 1.f, -2.5f, 3.25f };
	const Document	 jsonVec3 = vec3;
	REQUIRE( jsonVec3.is_array() );
	REQUIRE( jsonVec3.size() == 3 );
	CHECK( jsonVec3.get<VTX::Vec3f>() == vec3 );

	const VTX::Vec4i vec4 { 1, 2, 3, 4 };
	CHECK( Document( vec4 ).get<VTX::Vec4i>() == vec4 );

	const VTX::Quatf quat { 0.5f, 0.1f, 0.2f, 0.3f };
	const Document	 jsonQuat = quat;
	REQUIRE( jsonQuat.size() == 4 );
	CHECK( jsonQuat.at( 0 ).get<float>() == quat.x );
	CHECK( jsonQuat.at( 3 ).get<float>() == quat.w );
	CHECK( jsonQuat.get<VTX::Quatf>() == quat );

	VTX::Mat4f mat( 1.f );
	mat[ 3 ] = VTX::Vec4f { 10.f, 20.f, 30.f, 1.f };
	const Document jsonMat = mat;
	REQUIRE( jsonMat.size() == 4 );
	REQUIRE( jsonMat.at( 3 ).size() == 4 );
	CHECK( jsonMat.at( 3 ).at( 0 ).get<float>() == 10.f );
	CHECK( jsonMat.get<VTX::Mat4f>() == mat );
}

TEST_CASE( "Util::Json::Rgba", "[unit]" )
{
	using VTX::Util::Json::Document;

	const VTX::Util::Color::Rgba color { 0.25f, 0.5f, 0.75f, 0.5f };
	const Document				 json = color;
	REQUIRE( json.size() == 4 );
	CHECK( json.get<VTX::Util::Color::Rgba>().vec() == color.vec() );

	const Document jsonNoAlpha = Document::array( { 0.1f, 0.2f, 0.3f } );
	CHECK( jsonNoAlpha.get<VTX::Util::Color::Rgba>().vec().w == 1.f );
}

TEST_CASE( "Util::Json::Enum", "[unit]" )
{
	using VTX::Util::Json::Document;

	const Document json = E_JSON_TEST::BETA;
	REQUIRE( json.is_string() );
	CHECK( json.get<std::string>() == "BETA" );
	CHECK( json.get<E_JSON_TEST>() == E_JSON_TEST::BETA );

	CHECK_THROWS_AS( Document( "GAMMA" ).get<E_JSON_TEST>(), std::invalid_argument );

	const E_JSON_TEST_BITMASK flags	   = E_JSON_TEST_BITMASK::RED | E_JSON_TEST_BITMASK::BLUE;
	const Document			  jsonBits = flags;
	REQUIRE( jsonBits.is_array() );
	REQUIRE( jsonBits.size() == 2 );
	CHECK( jsonBits.at( 0 ).get<std::string>() == "RED" );
	CHECK( jsonBits.at( 1 ).get<std::string>() == "BLUE" );
	CHECK( jsonBits.get<E_JSON_TEST_BITMASK>() == flags );

	const Document jsonNone = E_JSON_TEST_BITMASK::NONE;
	CHECK( jsonNone.empty() );
	CHECK( jsonNone.get<E_JSON_TEST_BITMASK>() == E_JSON_TEST_BITMASK::NONE );

	CHECK_THROWS_AS( Document::array( { "RED", "PURPLE" } ).get<E_JSON_TEST_BITMASK>(), std::invalid_argument );
}

TEST_CASE( "Util::Json::File", "[unit]" )
{
	using VTX::Util::Json::Document;

	const VTX::FilePath dir( "data_test_json" );
	const VTX::FilePath path = dir / "sub" / "test.json";

	Document document;
	document[ "zeta" ]	= 1;
	document[ "alpha" ] = VTX::Vec3f { 1.f, 2.f, 3.f };
	document[ "enum" ]	= E_JSON_TEST::ALPHA;

	VTX::Util::Json::write( path, document );
	REQUIRE( std::filesystem::exists( path ) );

	const Document loaded = VTX::Util::Json::read( path );
	CHECK( loaded == document );
	CHECK( loaded.begin().key() == "zeta" );
	CHECK( loaded.at( "alpha" ).get<VTX::Vec3f>() == VTX::Vec3f { 1.f, 2.f, 3.f } );
	CHECK( loaded.at( "enum" ).get<E_JSON_TEST>() == E_JSON_TEST::ALPHA );

	const VTX::FilePath invalidPath = dir / "invalid.json";
	VTX::Util::Filesystem::writeFile( invalidPath, "{ \"key\": " );
	CHECK_THROWS_AS( VTX::Util::Json::read( invalidPath ), VTX::IOException );

	CHECK_THROWS_AS( VTX::Util::Json::read( dir / "missing.json" ), VTX::IOException );

	std::filesystem::remove_all( dir );
	CHECK( std::filesystem::exists( dir ) == false );
}
