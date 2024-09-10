#include <thread>
#include <ui/qt/util.hpp>
//
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/gateway/shared.hpp"
//
#include "tool/mdprep/gateway/backend_gromacs.hpp"
#include "tool/mdprep/gateway/engine_job_manager.hpp"
#include "tool/mdprep/ui/md_engine_field_placer.hpp"
#include "tool/mdprep/ui/md_engine_specific_field_placer.hpp"
//
#include "tool/mdprep/ui/gromacs_specific_field_placer.hpp"
//
#include "tool/mdprep/ui/gromacs_field_placer.hpp"
//
#include "tool/mdprep/ui/gromacs.hpp"
//
#include "tool/mdprep/ui/md_engine.hpp"

namespace VTX::Tool::Mdprep::ui
{
	class Gromacs::_Impl
	{
	  public:
		void get( Gateway::EngineJobManager & p_out ) noexcept { p_out = _jobManager; }
		void get( MdEngineFieldPlacer & p_out ) noexcept { p_out = GromacsFieldPlacer( _data ); }
		void get( const E_FIELD_SECTION & p_section, MdEngineSpecificFieldPlacer & p_out ) noexcept
		{
			switch ( p_section )
			{
			case E_FIELD_SECTION::system:
				p_out = MdEngineSpecificFieldPlacer( GromacsSystemFieldPlacer( _data.system ) );
				break;
			case E_FIELD_SECTION::minimization: break;
			case E_FIELD_SECTION::equilibrationNvt:
				p_out = MdEngineSpecificFieldPlacer( GromacsMdStepFieldPlacer( _data.nvt ) );
				break;
			case E_FIELD_SECTION::equilibrationNpt:
				p_out = MdEngineSpecificFieldPlacer( GromacsMdStepFieldPlacer( _data.npt ) );
				break;
			case E_FIELD_SECTION::production:
				p_out = MdEngineSpecificFieldPlacer( GromacsMdStepFieldPlacer( _data.prod ) );
				break;
			default: break;
			}
		}

	  private:
		Gateway::Gromacs::MdSettings _data;
		Gateway::Gromacs::JobManager _jobManager { _data };
	};

	Gromacs::Gromacs() : _impl( new _Impl() ) {}
	void Gromacs::get( MdEngineFieldPlacer & p_out ) noexcept { _impl->get( p_out ); }
	void Gromacs::get( const E_FIELD_SECTION & p_section, MdEngineSpecificFieldPlacer & p_out ) noexcept
	{
		_impl->get( p_section, p_out );
	}
	void Gromacs::get( Gateway::EngineJobManager & p_out ) noexcept { _impl->get( p_out ); }
	void get( const Gromacs &, Gateway::EngineSpecificCommonInformation & p_out ) noexcept
	{
		Gateway::Gromacs::get( p_out );
	}

	void Gromacs::Del::operator()( Gromacs::_Impl * p_ ) noexcept { delete p_; }
} // namespace VTX::Tool::Mdprep::ui
