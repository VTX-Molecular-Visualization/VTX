#include "tools/mdprep/gateway/shared.hpp"
//
#include "tools/mdprep/ui/screen_forms.hpp"

namespace VTX::Tool::Mdprep::ui
{
	ScreenForms::ScreenForms( Gateway::MdParameters & p_data, ValidationSignaler p_validation ) : _dataPtr( &p_data ) {}
} // namespace VTX::Tool::Mdprep::ui
