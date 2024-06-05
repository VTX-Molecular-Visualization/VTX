#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FIELD_PLACER_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FIELD_PLACER_GROMACS__

class QDoubleSpinBox;
class QComboBox;

namespace VTX::Tool::Mdprep::Gateway
{
	struct EngineSpecificCommonFormData;
	struct GromacsData;
} // namespace VTX::Tool::Mdprep::Gateway
namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;
	struct FormLayouts;
	class MdEngineSpecificFieldPlacer;
	// Class responsible for managing ui tied to gromacs
	class GromacsFieldPlacer
	{
	  public:
		GromacsFieldPlacer( FormLayouts );

		void get( MdEngineSpecificFieldPlacer & ) noexcept;
		void get( const EngineSpecificCommonFormData *& ) const noexcept;

		void activate() noexcept;
		void deactivate() noexcept;

	  private:
		EngineSpecificCommonFormData _commonData;
		GromacsData					 _specificData;
		FormLayouts					 _layouts;

		bool			 _activatedOnce	  = false;
		QDoubleSpinBox * _fieldConc		  = nullptr;
		QComboBox *		 _fieldWaterModel = nullptr;
		QComboBox *		 _fieldBoxShape	  = nullptr;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
