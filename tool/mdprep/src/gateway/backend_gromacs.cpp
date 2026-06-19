#include <algorithm>
#include <string>
#include <thread>
#include <util/logger.hpp>
#include <vector>
//
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/gateway/shared.hpp"
//
#include "tool/mdprep/gateway/backend_gromacs.hpp"
//
#include "tool/mdprep/actions/jobs.hpp"
#include "tool/mdprep/backends/gromacs/gromacs.hpp"
#include <app/action/action_manager.hpp>
#include <app/services.hpp>

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
		{
			p_out.push_back( string( translate( static_cast<E_BOX_SHAPE>( it_idx ) ) ) );
		}
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
		{
			p_.push_back( string( translate( static_cast<E_BOX_DIMENSION_MODE>( it_idx ) ) ) );
		}
	}

	void waterModelCollection( std::vector<std::string> & p_out ) noexcept
	{
		using namespace VTX::Tool::Mdprep::backends::Gromacs;
		for ( int it_idx = 0; it_idx < static_cast<int>( E_WATER_MODEL::COUNT ); it_idx++ )
		{
			p_out.push_back( string( static_cast<E_WATER_MODEL>( it_idx ) ) );
		}
	}

	namespace
	{
		void convert(
			const std::vector<backends::Gromacs::forcefield> & p_in,
			std::vector<std::string> &						   p_out
		) noexcept
		{
			using namespace VTX::Tool::Mdprep::backends::Gromacs;
			p_out.clear();
			for ( auto & it_ff : p_in )
			{
				auto ffName = it_ff.getName();
				p_out.push_back( { ffName.begin(), ffName.end() } );
			}
		}
	} // namespace

	void get( EngineSpecificCommonInformation & p_out ) noexcept
	{
		using namespace VTX::Tool::Mdprep::backends::Gromacs;
		auto ffs = listForcefields( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );
		convert( ffs, p_out.bioForcefieldCollection );
		boxShapeCollection( p_out.boxShapeCollection );
		waterModelCollection( p_out.waterModels );
	}

	JobManager::JobManager( MdSettings & p_data ) : _data( &p_data ) {}

	namespace
	{
		void convert( const std::string_view & p_in, backends::Gromacs::E_WATER_MODEL & out ) noexcept
		{
			using namespace backends::Gromacs;
			for ( int idx = 0; idx < static_cast<int>( E_WATER_MODEL::COUNT ); idx++ )
			{
				E_WATER_MODEL it = static_cast<E_WATER_MODEL>( idx );
				if ( string( it ) == p_in )
				{
					out = it;
					return;
				}
			}
		}

		void convert(
			const MdParameters &				p_param,
			const Gromacs::MdSettings &			p_gmxParam,
			backends::Gromacs::MdInstructions & p_out
		) noexcept
		{
			p_out.nvt.nsteps	= p_param.nvt.nsteps;
			p_out.nvt.dt		= p_param.nvt.dt;
			p_out.nvt.nstxout	= p_param.nvt.saveInterval;
			p_out.nvt.nstvout	= p_gmxParam.nvt.nstvout;
			p_out.nvt.nstenergy = p_gmxParam.nvt.nstenergy;
			p_out.nvt.nstlog	= p_gmxParam.nvt.nstlog;

			p_out.npt.nsteps	= p_param.npt.nsteps;
			p_out.npt.dt		= p_param.npt.dt;
			p_out.npt.nstxout	= p_param.npt.saveInterval;
			p_out.npt.nstvout	= p_gmxParam.npt.nstvout;
			p_out.npt.nstenergy = p_gmxParam.npt.nstenergy;
			p_out.npt.nstlog	= p_gmxParam.npt.nstlog;

			p_out.prod.nsteps			  = p_param.prod.nsteps;
			p_out.prod.dt				  = p_param.prod.dt;
			p_out.prod.nstxout			  = p_param.prod.saveInterval;
			p_out.prod.nstvout			  = p_gmxParam.prod.nstvout;
			p_out.prod.nstenergy		  = p_gmxParam.prod.nstenergy;
			p_out.prod.nstlog			  = p_gmxParam.prod.nstlog;
			p_out.prod.nstxout_compressed = p_gmxParam.prod.nstxout_compressed
												? p_gmxParam.prod.nstxout_compressed.value()
												: p_param.prod.saveInterval;

			p_out.fileStem = "your_system"; // TODO bad design
		}

		void convert(
			const MdParameters &										p_inGeneric,
			const Gromacs::MdSettings &									p_inGromacs,
			VTX::Tool::Mdprep::backends::Gromacs::GromacsInstructions & p_out
		) noexcept
		{
			convert( p_inGeneric.system.waterModel, p_out.pdb2gmx.water );
			p_out.pdb2gmx.forcefields = backends::Gromacs::listForcefields(
				executableDirectory() / backends::Gromacs ::defaultFfDirectoryRelativePath()
			);
			std::vector<std::string> strFfs;
			convert( p_out.pdb2gmx.forcefields, strFfs );
			auto hit = std::find( strFfs.begin(), strFfs.end(), p_inGeneric.system.forcefieldBio );
			if ( hit != strFfs.end() )
			{
				p_out.pdb2gmx.forcefieldIndex = std::distance( strFfs.begin(), hit );
			}
			else
			{
				p_out.pdb2gmx.forcefieldIndex = 0;
			}
			convert( p_inGeneric, p_inGromacs, p_out.mdInstructions );
		}
	} // namespace

	void JobManager::startPreparation( const MdParameters & p_1 ) noexcept
	{
		VTX::Tool::Mdprep::backends::Gromacs::GromacsInstructions gmxInstructions;
		convert( p_1, *_data, gmxInstructions );
		App::ACTION().execute<Actions::StartPreparation>( std::move( gmxInstructions ) );
	}

	bool JobManager::isResultAvailable() const noexcept { return _report != CheckReport(); }

	CheckReport JobManager::lastResult() const noexcept { return _report; }

	void JobManager::checkInputs( const MdParameters & p_1 ) noexcept
	{
		VTX::Tool::Mdprep::backends::Gromacs::GromacsInstructions gmxInstructions;
		convert( p_1, *_data, gmxInstructions );

		App::ACTION().execute<Actions::CheckSystem>( std::move( gmxInstructions ) );
	}

} // namespace VTX::Tool::Mdprep::Gateway::Gromacs
