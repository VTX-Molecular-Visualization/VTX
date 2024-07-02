#include <ui/qt/util.hpp>
//
#include "tools/mdprep/gateway/backend_gromacs.hpp"
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/gateway/shared.hpp"
#include "tools/mdprep/ui/md_engine_field_placer.hpp"
#include "tools/mdprep/ui/md_engine_specific_field_placer.hpp"
//
#include "tools/mdprep/ui/gromacs_field_placer.hpp"
#include "tools/mdprep/ui/gromacs_specific_field_placer.hpp"
//
#include "tools/mdprep/ui/gromacs.hpp"
//
#include "tools/mdprep/ui/md_engine.hpp"

namespace VTX::Tool::Mdprep::ui
{
	void Gromacs::get( MdEngineFieldPlacer & p_out ) noexcept { p_out = GromacsFieldPlacer( _data ); }
	void Gromacs::get( const E_FIELD_SECTION & p_section, MdEngineSpecificFieldPlacer & p_out ) noexcept
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
	void get( const Gromacs &, Gateway::EngineSpecificCommonInformation & p_out ) noexcept
	{
		Gateway::getGromacs( p_out );
	}
} // namespace VTX::Tool::Mdprep::ui
