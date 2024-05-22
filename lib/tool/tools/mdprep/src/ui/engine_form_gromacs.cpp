#include "tools/mdprep/ui/form_data.hpp"
//
#include "tools/mdprep/ui/engine_form_gromacs.hpp"
//
#include "tools/mdprep/gromacs/gromacs.hpp"

namespace VTX::Tool::Mdprep::ui
{
	EngineFormGromacs::EngineFormGromacs( FormLayouts p_layout ) : _layouts( std::move( p_layout ) )
	{
		using namespace VTX::Tool::Mdprep::Gromacs;
		auto ffs = listForcefields( VTX::Tool::Mdprep::executableDirectory() / defaultFfDirectoryRelativePath() );
		for ( auto & it_ff : ffs )
		{
			auto ffName = it_ff.getName();
			_commonData.bioForcefieldCollection.push_back( { ffName.begin(), ffName.end() } );
		}
	}
	void EngineFormGromacs::get( const EngineSpecificCommonFormData *& p_ptr ) const noexcept { p_ptr = &_commonData; }
	void EngineFormGromacs::activate() noexcept {}
	void EngineFormGromacs::deactivate() noexcept {}
} // namespace VTX::Tool::Mdprep::ui
