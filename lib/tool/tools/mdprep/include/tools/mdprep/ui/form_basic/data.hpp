#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC_DATA__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC_DATA__

#include <functional>

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParameters;
	struct EngineSpecificCommonInformation;
} // namespace VTX::Tool::Mdprep::Gateway
namespace VTX::Tool::Mdprep::ui
{
	class InputChecker;
	class MdEngineSpecificFieldPlacer;
	enum class E_FIELD_SECTION;
} // namespace VTX::Tool::Mdprep::ui

namespace VTX::Tool::Mdprep::ui::form_basic
{
	using SpecificFieldsPlacerCallback = std::function<MdEngineSpecificFieldPlacer( const E_FIELD_SECTION & )>;

	// Class responsible for providing data related to the form_basic features.
	class Data
	{
	  public:
		Data() = delete;
		Data(
			Gateway::MdParameters,
			InputChecker,
			SpecificFieldsPlacerCallback,
			Gateway::EngineSpecificCommonInformation p_info = Gateway::EngineSpecificCommonInformation()
		) noexcept;

		void get( Gateway::MdParameters *& ) noexcept;
		void get( Gateway::EngineSpecificCommonInformation *& ) noexcept;
		void get( InputChecker *& ) noexcept;
		void get( SpecificFieldsPlacerCallback *& ) noexcept;

	  private:
		struct _Data
		{
			Gateway::EngineSpecificCommonInformation lastFormData;
			Gateway::MdParameters					 parameters;
			InputChecker							 inputChecker;
			SpecificFieldsPlacerCallback			 specificFieldsCallback;
		};
		_Data _data;
	};
} // namespace VTX::Tool::Mdprep::ui::form_basic
#endif
