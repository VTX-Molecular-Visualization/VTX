#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_ADVANCED_FORM__
#define __VTX_TOOL_TOOLS_MDPREP_UI_ADVANCED_FORM__

#include <functional>

class QLineEdit;
class QComboBox;
class QLabel;
class QString;
class QWidget;

namespace VTX::Tool::Mdprep::ui
{
	struct MdParametersAdvanced;
	struct MdAdvancedDataSample;

	// Class Responsible for setting up the form for advanced MD parameters
	class MdAdvancedParamForm
	{
	  public:
		using UpdateCallback = std::function<void( const MdParametersAdvanced & )>;

		MdAdvancedParamForm() = default;

		void setupUi( QWidget * p_container, const MdParametersAdvanced & p_defaults = MdParametersAdvanced() );

		void subscribe( UpdateCallback ) noexcept;
		void get( MdParametersAdvanced & ) const noexcept;

	  private:
		void _createAndPlaceUiItems( QWidget * p_container ) noexcept;
		void _loadDefaultValues( const MdParametersAdvanced & p_defaults ) noexcept;
	};

} // namespace VTX::Tool::Mdprep::ui

#endif
