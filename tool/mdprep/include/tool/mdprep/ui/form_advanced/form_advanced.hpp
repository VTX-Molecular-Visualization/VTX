#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_ADVANCED_FORM_ADVANCED__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_ADVANCED_FORM_ADVANCED__

#include "tool/mdprep/ui/form_advanced/event_manager.hpp"
#include "tool/mdprep/ui/form_advanced/shared.hpp"
#include <array>
#include <functional>
#include <ui/qt/util.hpp>

class QLineEdit;
class QComboBox;
class QLabel;
class QFormLayout;
class QString;
class QWidget;
class QDoubleSpinBox;

namespace VTX::Util
{
	class Sentry;
}

namespace VTX::UI::QT::Util
{
	class LabelWithHelper;
	class ObjectOwnership;
} // namespace VTX::UI::QT::Util

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParameters;
	struct MdAdvancedDataSample;
	struct MdBasicDataSample;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	class MdEngineSpecificFieldPlacer;
	class InputChecker;
	class ReportManager;
} // namespace VTX::Tool::Mdprep::ui

namespace VTX::Tool::Mdprep::ui::form_advanced
{
	using namespace VTX::Tool::Mdprep::Gateway;

	class EventManager;
	struct UiData;

	// Class Responsible for setting up the form for advanced MD parameters
	class FormAdvanced
	{
	  public:
		using UpdateCallback			   = std::function<void( const MdAdvancedDataSample & )>;
		using SpecificFieldsPlacerCallback = std::function<MdEngineSpecificFieldPlacer( const E_FIELD_SECTION & )>;

		FormAdvanced() = delete;
		FormAdvanced(
			QWidget *					 p_container,
			SpecificFieldsPlacerCallback p_specificFieldGenerator,
			const MdParameters &		 p_defaults,
			ReportManager &				 p_reportManager

		);

		// Extract data and fill input with it.
		void get( MdParameters & ) const;

		// Fills input with Layouts this form uses.
		void get( FormLayouts & ) const noexcept;

		// Gives the form information about data common to every MdEngines
		void update( const EngineSpecificCommonInformation & p_data );

	  private:
		UiData											  _uiObjects;
		std::array<VTX::UI::QT::Util::ObjectOwnership, 2> _thingsToDelete;
		UpdateCallback									  _updateCallback = []( const MdAdvancedDataSample & ) {};
		EngineSpecificCommonInformation					  _lastUpdateData;
		SpecificFieldsPlacerCallback					  _specificFieldGenerator;
		IonicFields										  _fieldsIons;
		mutable EventManager							  _eventManager;

		MdParameters & _parameters() { return _eventManager.parameters; }

		MdEngineSpecificFieldPlacer & _fieldPlacer( const size_t & );
		void						  _createAndPlaceUiItems( QWidget * p_container );
		QWidget *					  _uiSetupContainer( QWidget * p_container );
		void						  _uiAddSystem( QVBoxLayout * p_target );
		void						  _uiAddExpertButton( QVBoxLayout * p_target );
		void						  _uiAddMinimization( QVBoxLayout * p_target );
		void _uiAddStep( QVBoxLayout * p_target, const char * p_header, UiData::FieldSet & p_field );
		void _loadValues( const MdParameters & p_defaults );
		void _fillEngineSpecificData();
	};

} // namespace VTX::Tool::Mdprep::ui::form_advanced

#endif
