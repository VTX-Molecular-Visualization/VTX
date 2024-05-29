
#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_ENGINE_FORM_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_UI_ENGINE_FORM_GROMACS__

class QDoubleSpinBox;
class QComboBox;

namespace VTX::Tool::Mdprep::ui
{
	struct EngineSpecificCommonFormData;
	struct FormLayouts;

	// Class responsible for managing ui tied to gromacs
	class EngineFormGromacs
	{
		EngineSpecificCommonFormData _commonData;
		FormLayouts					 _layouts;

		bool			 _activatedOnce	  = false;
		QDoubleSpinBox * _fieldConc		  = nullptr;
		QComboBox *		 _fieldWaterModel = nullptr;
		QComboBox *		 _fieldBoxShape	  = nullptr;

	  public:
		EngineFormGromacs( FormLayouts );

		void get( const EngineSpecificCommonFormData *& ) const noexcept;

		void activate() noexcept;
		void deactivate() noexcept;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
