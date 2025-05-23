#include <catch2/catch_test_macros.hpp>
#include <util/url.hpp>

TEST_CASE( "Util::Url", "[url]" )
{
	using namespace VTX::Util::Url;

	SystemId id { "1AGA" };
	CHECK( id.str == "1AGA" );

	std::string t { "a" };
	t += systemReplacementToken();
	t += "z";
	UrlTemplate templat { t.data() };
	CHECK( templat.str == t );
	CHECK( templat.hasReplacementToken() );

	UrlTemplate badTemplat { "poney" };
	CHECK( not badTemplat.hasReplacementToken() );

	UrlFull full { id };
	CHECK( full.str == ( std::string( rcsbPdbDownloadBaseUrl() ) + id.str + rcsbPdbDownloadFileExtension() ) );
}
