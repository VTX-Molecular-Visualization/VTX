#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/gateway/shared.hpp"
#include "tools/mdprep/ui/input_checker.hpp"
//
#include "tools/mdprep/ui/form_basic/data.hpp"

namespace VTX::Tool::Mdprep::ui::form_basic
{
	Data::Data(
		Gateway::MdParameters					 p_param,
		InputChecker							 p_inputChecker,
		SpecificFieldsPlacerCallback			 p_specificFieldsCallback,
		Gateway::EngineSpecificCommonInformation p_info
	) noexcept :
		_data( _Data { .lastFormData		   = std::move( p_info ),
					   .parameters			   = std::move( p_param ),
					   .inputChecker		   = std::move( p_inputChecker ),
					   .specificFieldsCallback = std::move( p_specificFieldsCallback ) } )
	{
	}
	void Data::get( Gateway::MdParameters *& p_out ) noexcept { p_out = &_data.parameters; }
	void Data::get( Gateway::EngineSpecificCommonInformation *& p_out ) noexcept { p_out = &_data.lastFormData; }
	void Data::get( InputChecker *& p_out ) noexcept { p_out = &_data.inputChecker; }
	void Data::get( SpecificFieldsPlacerCallback *& p_out ) noexcept { p_out = &_data.specificFieldsCallback; }
} // namespace VTX::Tool::Mdprep::ui::form_basic
