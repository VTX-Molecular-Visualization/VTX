#include <catch2/catch_test_macros.hpp>
#include <util/math.hpp>

namespace
{
	bool closeEnough( const float & v1, const float & v2 ) { return std::abs( v1 - v2 ) < 0.001f; }
	bool checkTorsion(
		const VTX::Vec3f & p1,
		const VTX::Vec3f & p2,
		const VTX::Vec3f & p3,
		const VTX::Vec3f & p4,
		const float &	   expectedValue
	)
	{
		// return closeEnough( VTX::Util::Math::_torsionalAngle( p1, p2, p3, p4 ), expectedValue );
		return closeEnough( VTX::Util::Math::torsionalAngle( p1, p2, p3, p4 ), expectedValue );
	}
} // namespace

TEST_CASE( "Util::Math::torsionalAngle", "[math]" )
{
	using namespace VTX;

	CHECK( checkTorsion( Vec3f( -1, 0, 0 ), Vec3f( 0, 0, 0 ), Vec3f( 0, 1, 0 ), Vec3f( 0, 1, 1 ), VTX::PI_2f ) );
	CHECK( checkTorsion( Vec3f( -1, 0, 0 ), Vec3f( 0, 0, 0 ), Vec3f( 0, 1, 0 ), Vec3f( -1, 1, 1 ), VTX::PI_2f / 2.f ) );
	CHECK(
		checkTorsion( Vec3f( -1, 0, 0 ), Vec3f( 0, 0, 0 ), Vec3f( 0, 1, 0 ), Vec3f( -1, 1, -1 ), -( VTX::PI_2f / 2.f ) )
	);
	CHECK(
		checkTorsion( Vec3f( -1, 0, 0 ), Vec3f( 0, 0, 0 ), Vec3f( 0, 1, 0 ), Vec3f( -1, 2, -1 ), -( VTX::PI_2f / 2.f ) )
	);
	CHECK( checkTorsion(
		Vec3f( -1, -1, 0 ), Vec3f( 0, 0, 0 ), Vec3f( 0, 1, 0 ), Vec3f( -1, 2, -1 ), -( VTX::PI_2f / 2.f )
	) );
}
