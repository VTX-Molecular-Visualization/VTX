#include <Eigen/Geometry>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "VTX_TOOL - eigen", "[integration]" )
{
	const int		 ROW_COUNT = 3;
	const int		 COL_COUNT = 5;
	Eigen::Matrix3Xd matTest   = Eigen::Matrix3Xd( ROW_COUNT, COL_COUNT );

	for ( int i = 0; i < ROW_COUNT; i++ )
		for ( int j = 0; j < COL_COUNT; j++ )
			matTest.coeffRef( i, j ) = i * j;

	REQUIRE( matTest.coeff( 1, 1 ) == 1 );
}

// TEST_CASE( "VTX_UI - Benchmark", "[.][perfs]" ) {}
