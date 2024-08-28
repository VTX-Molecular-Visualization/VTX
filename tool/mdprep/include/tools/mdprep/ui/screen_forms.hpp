#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_SCREEN_FORMS__
#define __VTX_TOOL_TOOLS_MDPREP_UI_SCREEN_FORMS__

#include "tools/mdprep/ui/md_engine_factory.hpp"
#include <functional>
#include <optional>

class QComboBox;
class QWidget;

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParameters;
	class EngineJobManager;
	class JobUpdateIntermediate;
} // namespace VTX::Tool::Mdprep::Gateway
namespace VTX::Tool::Mdprep::ui
{
	class FormSwitchButton;
	class Form;
	class ReportManager;
	class MdEngineFieldPlacer;
	class MdEngineSpecificFieldPlacer;
	enum class E_FIELD_SECTION;

	// Class responsible for signaling that a form is validated
	class ValidationSignaler
	{
	  public:
		ValidationSignaler( std::function<void( Gateway::JobUpdateIntermediate )> );

		void preparationStarted( Gateway::JobUpdateIntermediate ) noexcept;

	  private:
		std::function<void( Gateway::JobUpdateIntermediate )> _callback;
	};

	// Class responsible for setting up the form screens, allowing user to fill it and start the processing
	class ScreenForms
	{
		inline static const QSize PREFERRED_SIZE { 500, 720 };

	  public:
		ScreenForms() = delete;
		ScreenForms( QWidget * p_parent, Gateway::MdParameters &, ValidationSignaler );

	  private:
		using EngineCollection
			= std::array<std::optional<VTX::Tool::Mdprep::ui::MdEngine>, VTX::Tool::Mdprep::ui::MD_ENGINE_NUMBER>;

		Gateway::MdParameters * _dataPtr = nullptr;
		ValidationSignaler		_validationSignaler;

		QComboBox *	  _w_mdEngine	 = nullptr;
		QWidget *	  _formContainer = nullptr;
		QPushButton * _buttonStart	 = nullptr;

		// VTX::Tool::Mdprep::ui::FormSwitchButton   _fieldOrganizer;
		VTX::Tool::Mdprep::ui::FormSwitchButton				_switchButton;
		VTX::Tool::Mdprep::ui::Form							_currentForm;
		EngineCollection									_mdEngines;
		int													_mdEngineCurrentIdx = 0;
		VTX::Tool::Mdprep::Gateway::EngineJobManager		_jobManager;
		std::optional<VTX::Tool::Mdprep::ui::ReportManager> _reportManager;
		VTX::Tool::Mdprep::ui::MdEngineFieldPlacer			_formEngine;

		void _updateMdEngine( int idx ) noexcept;
		void _setupUi( QWidget * p_parent ) noexcept;
		void _updateForm() noexcept;
		void _setFormBasic() noexcept;
		void _setFormAdvanced() noexcept;
		void _startPreparation() noexcept;
		void _setupSlots() noexcept;

		std::function<MdEngineSpecificFieldPlacer( const E_FIELD_SECTION & )> _SpecificFieldPlacerGetter() noexcept;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
