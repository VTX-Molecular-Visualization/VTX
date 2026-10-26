#ifndef VTX_TOOL_MDPREP_UI_FORM_BASIC_DATA_HPP
#define VTX_TOOL_MDPREP_UI_FORM_BASIC_DATA_HPP

#include <functional>

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParameters;
	struct EngineSpecificCommonInformation;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	class ReportManager;
	class MdEngineSpecificFieldPlacer;
	enum class E_FIELD_SECTION : uint8_t;
} // namespace VTX::Tool::Mdprep::ui

namespace VTX::Tool::Mdprep::ui::form_basic
{
	using SpecificFieldsPlacerCallback = std::function<MdEngineSpecificFieldPlacer( const E_FIELD_SECTION & )>;

	// Class responsible for providing data related to the form_basic features.
	class Data
	{
	  public:
		Data() = delete;
		Data( Gateway::MdParameters,
			  ReportManager &,
			  SpecificFieldsPlacerCallback,
			  Gateway::EngineSpecificCommonInformation p_info = Gateway::EngineSpecificCommonInformation() ) noexcept;

		void get( Gateway::MdParameters *& ) noexcept;
		void get( Gateway::EngineSpecificCommonInformation *& ) noexcept;
		void get( ReportManager *& ) const noexcept;
		void get( SpecificFieldsPlacerCallback *& ) noexcept;

	  private:
		struct Impl
		{
			Gateway::EngineSpecificCommonInformation lastFormData;
			Gateway::MdParameters					 parameters;
			ReportManager *							 reportManager;
			SpecificFieldsPlacerCallback			 specificFieldsCallback;
		};

		Impl _data;
	};
} // namespace VTX::Tool::Mdprep::ui::form_basic
#endif
