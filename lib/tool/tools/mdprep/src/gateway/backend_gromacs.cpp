#include <string>
#include <thread>
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
	void boxShapeCollection( std::vector<std::string> & p_out ) noexcept
	{
		using namespace VTX::Tool::Mdprep::backends::Gromacs;
		for ( int it_idx = 0; it_idx < static_cast<int>( E_EDITCONF_BOX_SHAPE::COUNT ); it_idx++ )
			p_out.push_back( string( static_cast<E_EDITCONF_BOX_SHAPE>( it_idx ) ) );
	}
	void boxDimensionModeCollection( std::vector<std::string> & p_ ) noexcept
	{
		using namespace VTX::Tool::Mdprep::backends::Gromacs;
		for ( int it_idx = 0; it_idx < static_cast<int>( E_EDITCONF_BOX_DIMENSION_MODE::COUNT ); it_idx++ )
			p_.push_back( string( static_cast<E_EDITCONF_BOX_DIMENSION_MODE>( it_idx ) ) );
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
		void fakeCheck( CheckReportCallback p_callback )
		{
			std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
			p_callback( CheckReport { .itemGeneric = E_REPORT_CHECKED_ITEM::systemWithForceField,
									  .message	   = "Fakely checked up." } );
		}
	} // namespace

	void JobManager::checkInputs( const MdParameters & p_1, CheckReportCallback p_2 ) const noexcept
	{
		if ( _thr.joinable() )
			_thr.join();
		_thr = std::thread( [ callback = std::move( p_2 ) ]() { fakeCheck( std::move( callback ) ); } );
	}

	void JobManager::startPreparation( const MdParameters & p_1, JobUpdateCallback p_3 ) noexcept {}

} // namespace VTX::Tool::Mdprep::Gateway::Gromacs
