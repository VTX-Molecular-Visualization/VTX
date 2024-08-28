#include <string>
#include <thread>
#include <util/logger.hpp>
#include <vector>
//
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/gateway/shared.hpp"
//
#include "tools/mdprep/gateway/backend_gromacs.hpp"
//
#include "tools/mdprep/backends/gromacs/gromacs.hpp"

namespace VTX::Tool::Mdprep::Gateway::Gromacs
{
	backends::Gromacs::E_EDITCONF_BOX_SHAPE translate( const E_BOX_SHAPE & p_ ) noexcept
	{
		switch ( p_ )
		{
		case E_BOX_SHAPE::dodecahedron: return backends::Gromacs::E_EDITCONF_BOX_SHAPE ::dodecahedron;
		case E_BOX_SHAPE::cubic: return backends::Gromacs::E_EDITCONF_BOX_SHAPE ::cubic;
		case E_BOX_SHAPE::octahedron: return backends::Gromacs::E_EDITCONF_BOX_SHAPE ::octahedron;
		case E_BOX_SHAPE::triclinic: return backends::Gromacs::E_EDITCONF_BOX_SHAPE ::triclinic;
		case E_BOX_SHAPE::COUNT: return backends::Gromacs::E_EDITCONF_BOX_SHAPE ::COUNT;
		default: return backends::Gromacs::E_EDITCONF_BOX_SHAPE ::dodecahedron;
		}
	}
	void boxShapeCollection( std::vector<std::string> & p_out ) noexcept
	{
		using namespace VTX::Tool::Mdprep::backends::Gromacs;
		for ( int it_idx = 0; it_idx < static_cast<int>( E_BOX_SHAPE::COUNT ); it_idx++ )
			p_out.push_back( string( translate( static_cast<E_BOX_SHAPE>( it_idx ) ) ) );
	}

	backends::Gromacs::E_EDITCONF_BOX_DIMENSION_MODE translate( const E_BOX_DIMENSION_MODE & p_ ) noexcept
	{
		switch ( p_ )
		{
		case E_BOX_DIMENSION_MODE::distance: return backends::Gromacs::E_EDITCONF_BOX_DIMENSION_MODE ::distance;
		case E_BOX_DIMENSION_MODE::size: return backends::Gromacs::E_EDITCONF_BOX_DIMENSION_MODE ::size;
		default: return backends::Gromacs::E_EDITCONF_BOX_DIMENSION_MODE ::distance;
		}
	}

	void boxDimensionModeCollection( std::vector<std::string> & p_ ) noexcept
	{
		using namespace VTX::Tool::Mdprep::backends::Gromacs;
		for ( int it_idx = 0; it_idx < static_cast<int>( E_BOX_DIMENSION_MODE::COUNT ); it_idx++ )
			p_.push_back( string( translate( static_cast<E_BOX_DIMENSION_MODE>( it_idx ) ) ) );
	}
	void waterModelCollection( std::vector<std::string> & p_out ) noexcept
	{
		using namespace VTX::Tool::Mdprep::backends::Gromacs;
		for ( int it_idx = 0; it_idx < static_cast<int>( E_WATER_MODEL::COUNT ); it_idx++ )
			p_out.push_back( string( static_cast<E_WATER_MODEL>( it_idx ) ) );
	}
	void get( EngineSpecificCommonInformation & p_out ) noexcept
	{
		using namespace VTX::Tool::Mdprep::backends::Gromacs;
		auto ffs = listForcefields( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );
		for ( auto & it_ff : ffs )
		{
			auto ffName = it_ff.getName();
			p_out.bioForcefieldCollection.push_back( { ffName.begin(), ffName.end() } );
		}
		boxShapeCollection( p_out.boxShapeCollection );
		waterModelCollection( p_out.waterModels );
	}

	JobManager::JobManager( MdSettings & p_data ) : _data( &p_data ) {}

	namespace
	{
		void fakeCheck( CheckReport & p_result, CheckReportCallback p_callback )
		{
			VTX::VTX_DEBUG( "Starting fake check" );
			std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
			p_result = CheckReport { .itemGeneric = E_REPORT_CHECKED_ITEM::systemWithForceField,
									 .message	  = "Fakely checked up." };
			p_callback( p_result );
			VTX::VTX_DEBUG( "Fake check finished" );
		}
	} // namespace

	void JobManager::checkInputs( const MdParameters & p_1, CheckReportCallback p_2 ) const noexcept
	{
		_threadStack.push( std::jthread( [ callback = std::move( p_2 ), report = &_report ]()
										 { fakeCheck( *report, std::move( callback ) ); } ) );
	}

	namespace
	{
		void fakePreparation( const MdParameters & p_1, JobUpdateCallback p_2 )
		{
			VTX::VTX_DEBUG( "Starting fake Prepartion" );
			std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
			p_2( { "Starting ..." } );

			VTX::VTX_DEBUG( "Fake Prepartion - step 1" );
			std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
			p_2( { "Step 1 done" } );

			VTX::VTX_DEBUG( "Fake Prepartion - step 2" );
			std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
			p_2( { "Step 2 done" } );

			VTX::VTX_DEBUG( "Fake Prepartion - step 3" );
			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
			p_2( { "Step 3 done" } );

			VTX::VTX_DEBUG( "Fake Prepartion - step 5" );
			std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
			p_2( { "Step 4 done" } );

			VTX::VTX_DEBUG( "Fake Prepartion - step 6" );
			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
			p_2( { "Step 5 done" } );

			VTX::VTX_DEBUG( "Fake Prepartion - step 7" );
			std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
			p_2( { "Step 6 done" } );

			VTX::VTX_DEBUG( "Finishing fake Prepartion" );
			p_2( { "Preparation finished" } );
		}
	} // namespace

	void JobManager::startPreparation( const MdParameters & p_1, JobUpdateCallback p_3 ) noexcept
	{
		_threadStack.push( std::jthread { [ &params = p_1, callback = std::move( p_3 ) ]
										  { fakePreparation( params, std::move( callback ) ); } } );
	}

	bool JobManager::isResultAvailable() const noexcept { return _report != CheckReport(); }

	CheckReport JobManager::lastResult() const noexcept { return _report; }

} // namespace VTX::Tool::Mdprep::Gateway::Gromacs
