#include <catch2/catch_test_macros.hpp>
#include <util/color/rgba.hpp>

TEST_CASE( "Util::Color::Rgba::Operators", "[unit]" )
{
	using VTX::Util::Color::Rgba;

	const Rgba a { 0.5f, 0.25f, 0.125f, 0.5f };
	const Rgba b { 0.25f, 0.5f, 0.25f, 0.25f };

	CHECK( a + b == Rgba( 0.75f, 0.75f, 0.375f, 0.75f ) );
	CHECK( a - b == Rgba( 0.25f, -0.25f, -0.125f, 0.25f ) );
	CHECK( a * b == Rgba( 0.125f, 0.125f, 0.03125f, 0.125f ) );
	CHECK( a / b == Rgba( 2.f, 0.5f, 0.5f, 2.f ) );

	CHECK( a * 2.f == Rgba( 1.f, 0.5f, 0.25f, 1.f ) );
	CHECK( 2.f * a == Rgba( 1.f, 0.5f, 0.25f, 1.f ) );
	CHECK( a + 0.5f == Rgba( 1.f, 0.75f, 0.625f, 1.f ) );
	CHECK( 1.f - a == Rgba( 0.5f, 0.75f, 0.875f, 0.5f ) );
	CHECK( a / 0.5f == Rgba( 1.f, 0.5f, 0.25f, 1.f ) );

	Rgba c = a;
	c += b;
	CHECK( c == a + b );
	c -= b;
	CHECK( c == a );
	c *= 2.f;
	CHECK( c == a * 2.f );
	c /= 2.f;
	CHECK( c == a );
}

TEST_CASE( "Util::Color::Rgba::Alpha", "[unit]" )
{
	using VTX::Util::Color::Rgba;

	Rgba color { 1.5f, -0.5f, 0.5f, 2.f };
	color.saturate();
	CHECK( color == Rgba( 1.f, 0.f, 0.5f, 1.f ) );

	const Rgba transparent { 0.2f, 0.4f, 0.1f, 0.5f };
	CHECK( transparent.toHighContrast().a() == 0.5f );
	CHECK( transparent.toColorblind().a() == 0.5f );
	CHECK( transparent.mixedWith( Rgba( 1.f, 1.f, 1.f ), 0.5f ).a() == 0.5f );

	CHECK( transparent.toStdVector().size() == 4 );
	CHECK( Rgba::randomPastel().a() == 1.f );
}
