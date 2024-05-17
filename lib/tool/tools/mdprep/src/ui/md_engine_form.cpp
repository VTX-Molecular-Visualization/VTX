#include "tools/mdprep/ui/md_engine_form.hpp"
#include "tools/mdprep/ui/form_data.hpp"
#include <string>
namespace VTX::Tool::Mdprep::ui
{

	void MdEngineForm::get( const EngineSpecificCommonFormData *& p_data ) noexcept { _ptr->get( p_data ); }
	void MdEngineForm::activate() noexcept { _ptr->activate(); }
	void MdEngineForm::deactivate() noexcept { _ptr->deactivate(); }
} // namespace VTX::Tool::Mdprep::ui
