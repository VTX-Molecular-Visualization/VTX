#include <catch2/catch_test_macros.hpp>
#include <util/color/rgba.hpp>
#include <util/fmt/glm.hpp>
#include <util/fmt/range.hpp>
#include <util/fmt/rgba.hpp>
#include <util/logger.hpp>
#include <util/math.hpp>
#include <util/math/range.hpp>
#include <util/types.hpp>

TEST_CASE( "Util::Fmt", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::Util;

	VTX::VTX_INFO( "info with Vec2: {0} / {1}", Vec2i( 0, 1 ), Vec2f( 0.1f, 10.4f ) );

	VTX::VTX_INFO(
		"info with Vec3: {0} / {1} / {2} / {3} / {4:.3f}",
		Vec3i( -1, 0, 1 ),
		Vec3u( -1, 0, 1 ),
		Vec3b( true, true, false ),
		Vec3f( 4 / 3.f, 0.f, 1.6f ),
		Vec3d( 4 / 3., 0.f, 1.6 )
	);

	VTX::VTX_INFO(
		"info with Vec4: {0} / {1} / {2} / {3} / {4:.3f}",
		Vec4i( 0, -1, 0, 1 ),
		Vec4u( 0, -1, 0, 1 ),
		Vec4b( false, true, true, false ),
		Vec4f( 10.55f, 4 / 3.f, 0.f, 1.6f ),
		Vec4d( 10.55, 4 / 3., 0.f, 1.6 )
	);

	VTX::VTX_INFO( "Mat3x3: {0:.2f}", Mat3f( 0, 1, 2, 3, 4, 5, 6, 7, 8 ) );
	VTX::VTX_INFO( "Mat4x3: {0:.2f}", Mat43f( 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ) );
	VTX::VTX_INFO( "Mat4x4: {0:.2f}", Mat4f( 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 ) );

	VTX::VTX_INFO( "Quatf: {0}", Quatf( 0.f, 1.f, 1 / 3.f, 1 / 2.f ) );
	VTX::VTX_INFO( "Quatd: {0}", Quatd( 0., 1., 1 / 3., 1 / 2. ) );

	VTX::VTX_INFO( "Range: {0}", Math::Range<int>::createFirstLast( 8, 10 ) );
	VTX::VTX_INFO( "Color: {0}", Color::Rgba( 255, 128, 0, 255 ) );
}
