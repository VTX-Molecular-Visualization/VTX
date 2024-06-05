#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FIELD_PLACER_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FIELD_PLACER_GROMACS__

class QDoubleSpinBox;
class QComboBox;

namespace VTX::Tool::Mdprep::Gateway
{
	struct EngineSpecificCommonFormData;

}
namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;
	struct FormLayouts;
	class MdEngineSpecificFieldPlacer;
	// Class responsible for managing ui tied to gromacs
	class GromacsFieldPlacer
	{
		EngineSpecificCommonFormData _commonData;
		FormLayouts					 _layouts;

		bool			 _activatedOnce	  = false;
		QDoubleSpinBox * _fieldConc		  = nullptr;
		QComboBox *		 _fieldWaterModel = nullptr;
		QComboBox *		 _fieldBoxShape	  = nullptr;

	  public:
		GromacsFieldPlacer( FormLayouts );

		void get( const MdEngineSpecificFieldPlacer *& ) const noexcept;
		void get( const EngineSpecificCommonFormData *& ) const noexcept;

		void activate() noexcept;
		void deactivate() noexcept;

	  private:
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
