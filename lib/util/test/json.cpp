#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <optional>
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

	using VTX::Util::Color::Rgba;

	const Rgba	   opaque { 18, 20, 24 };
	const Document jsonOpaque = opaque;
	REQUIRE( jsonOpaque.is_string() );
	CHECK( jsonOpaque.get<std::string>() == "#121418FF" );
	CHECK( jsonOpaque.get<Rgba>() == opaque );

	const Rgba	   transparent { 255, 64, 0, 128 };
	const Document jsonTransparent = transparent;
	CHECK( jsonTransparent.get<std::string>() == "#FF400080" );
	CHECK( jsonTransparent.get<Rgba>() == transparent );

	CHECK( Document( "#ff4000" ).get<Rgba>() == Rgba( 255, 64, 0 ) );
	CHECK( Rgba( 1.5f, -0.2f, 0.5f ).toHexaString() == "#FF0080FF" );
	CHECK( Rgba::toHexaString( transparent ) == "#FF400080" );
	CHECK( Rgba( std::string_view( "#FF400080" ) ) == transparent );

	Rgba color;
	color.fromHexaString( "#121418" );
	CHECK( color == opaque );

	Rgba staticColor;
	Rgba::fromHexaString( "#FF400080", staticColor );
	CHECK( staticColor == transparent );
	CHECK_THROWS_AS( color.fromHexaString( "#12141" ), std::invalid_argument );

	CHECK_THROWS_AS( Document( "FF4000" ).get<Rgba>(), std::invalid_argument );
	CHECK_THROWS_AS( Document( "#FF40" ).get<Rgba>(), std::invalid_argument );
	CHECK_THROWS_AS( Document( "#FF40GG" ).get<Rgba>(), std::invalid_argument );
	CHECK_THROWS_AS( Document( "#-F4000" ).get<Rgba>(), std::invalid_argument );
	CHECK_THROWS_AS( Document::array( { 0.1f, 0.2f, 0.3f } ).get<Rgba>(), nlohmann::json::type_error );
}

TEST_CASE( "Util::Json::RoundTrip", "[unit]" )
{
	using VTX::Util::Color::Rgba;
	using VTX::Util::Json::Document;

	const auto roundTrip = []<typename T>( const T & p_value ) -> T
	{ return Document::parse( Document( p_value ).dump() ).template get<T>(); };

	for ( int channel = 0; channel < 256; ++channel )
	{
		const Rgba color { channel, 255 - channel, channel, 255 - channel };
		Rgba	   current = color;
		for ( int i = 0; i < 5; ++i )
		{
			current = roundTrip( current );
			REQUIRE( current == color );
		}
	}

	const Rgba arbitrary { 0.62f, 0.37f, 0.123456f, 0.999f };
	const Rgba quantized = roundTrip( arbitrary );
	CHECK( quantized != arbitrary );
	CHECK( roundTrip( quantized ) == quantized );
	CHECK( roundTrip( roundTrip( quantized ) ) == quantized );

	for ( const float value : { 0.f, 0.1f, 0.15f, 1.4f, -2.5f, 3.14159265f, 1e-7f, 123456.789f } )
	{
		float current = value;
		for ( int i = 0; i < 5; ++i )
		{
			current = roundTrip( current );
			REQUIRE( current == value );
		}
	}

	const VTX::Vec3f vec { 0.1f, 0.2f, 0.3f };
	CHECK( roundTrip( roundTrip( vec ) ) == vec );
}

TEST_CASE( "Util::Json::Optional", "[unit]" )
{
	using VTX::Util::Json::Document;

	const std::optional<VTX::Vec3f> value { VTX::Vec3f { 1.f, 2.f, 3.f } };
	const std::optional<VTX::Vec3f> empty {};

	const Document jsonValue = value;
	const Document jsonEmpty = empty;
	CHECK( jsonValue.is_array() );
	CHECK( jsonEmpty.is_null() );
	CHECK( jsonValue.get<std::optional<VTX::Vec3f>>() == value );
	CHECK( jsonEmpty.get<std::optional<VTX::Vec3f>>() == empty );

	Document object;
	object[ "present" ] = value;
	object[ "null" ]	= empty;
	CHECK( object.value( "present", empty ) == value );
	CHECK( object.value( "null", value ) == empty );
	CHECK( object.value( "missing", value ) == value );
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
