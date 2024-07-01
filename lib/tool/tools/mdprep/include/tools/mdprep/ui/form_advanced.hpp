#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_ADVANCED__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_ADVANCED__

#include <functional>

class QLineEdit;
class QComboBox;
class QLabel;
class QString;
class QWidget;
class QDoubleSpinBox;

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParameters;
	struct MdAdvancedDataSample;
	struct MdBasicDataSample;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	class MdEngineSpecificFieldPlacer;

	// Class Responsible for setting up the form for advanced MD parameters
	class FormAdvanced
	{
	  public:
		using UpdateCallback			   = std::function<void( const MdAdvancedDataSample & )>;
		using SpecificFieldsPlacerCallback = std::function<MdEngineSpecificFieldPlacer( const E_FIELD_SECTION & )>;

		FormAdvanced() = delete;
		FormAdvanced( QWidget * p_container, const MdParameters & p_defaults = MdParameters() );
		FormAdvanced & operator=( const FormAdvanced & ) = delete;
		FormAdvanced( const FormAdvanced & )			 = delete;
		FormAdvanced & operator=( FormAdvanced && )		 = default;
		FormAdvanced( FormAdvanced && )					 = default;
		~FormAdvanced();

		// Extract data and fill input with it.
		void get( MdParameters & ) const noexcept;

		// Fills input with Layouts this form uses.
		void get( FormLayouts & ) const noexcept;

		// Gives the form information about data common to every MdEngines
		void update( const EngineSpecificCommonInformation & p_data ) noexcept;

	  private:
		UpdateCallback					_updateCallback = []( const MdAdvancedDataSample & ) {};
		MdParameters					_parameters;
		EngineSpecificCommonInformation _lastUpdateData;

		struct UiData
		{
			QWidget * container = nullptr;

			QFormLayout * _layoutMinimization				= nullptr;
			QLineEdit *	  _fieldMinimizationStepSize		= nullptr;
			QLineEdit *	  _fieldMinimizationStopThreshold	= nullptr;
			QLineEdit *	  _fieldProductionNstxoutCompressed = nullptr;
			struct FieldSet
			{
				QFormLayout *	 layout		 = nullptr;
				QLineEdit *		 nsteps		 = nullptr;
				QLabel *		 labelNsteps = nullptr;
				QDoubleSpinBox * dt			 = nullptr;
				QLineEdit *		 nstxout	 = nullptr;
				QLineEdit *		 nstvout	 = nullptr;
				QLineEdit *		 nstenergy	 = nullptr;
				QLineEdit *		 nstlog		 = nullptr;
			};
			FieldSet _equilibrationNvt;
			FieldSet _equilibrationNpt;
			FieldSet _production;

			QFormLayout * _layoutSystemPreparation = nullptr;
		} _uiObjects;

		void _createAndPlaceUiItems( QWidget * p_container ) noexcept;
		void _loadValues( const MdParameters & p_defaults ) noexcept;
		void _sendUpdateSignal() noexcept;

		// Class responsible for turning on and off events on the ui
		class EventToggler
		{
		  public:
			EventToggler( std::function<void()> );

			void toggle( UiData & ) noexcept;

		  private:
			std::function<void()> _lambdaToConnect;
			bool				  _eventsOn = false;

			void _connect( UiData & ) noexcept;
			void _disconnect( UiData & ) noexcept;
		};
		EventToggler _eventToggler { [ & ]() { this->_sendUpdateSignal(); } };
	};

} // namespace VTX::Tool::Mdprep::ui

#endif
