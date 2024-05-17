#include "tools/mdprep/ui/form_data.hpp"
//
#include "tools/mdprep/ui/engine_form_gromacs.hpp"

namespace VTX::Tool::Mdprep::ui
{
	EngineFormGromacs::EngineFormGromacs( FormLayouts p_layout ) : _layouts( std::move( p_layout ) ) {}
	void EngineFormGromacs::get( const EngineSpecificCommonFormData *& p_ptr ) const noexcept { p_ptr = &_commonData; }
	void EngineFormGromacs::activate() noexcept {}
	void EngineFormGromacs::deactivate() noexcept {}
} // namespace VTX::Tool::Mdprep::ui
