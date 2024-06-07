#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FIELD_PLACER_GROMACS__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FIELD_PLACER_GROMACS__

class QDoubleSpinBox;
class QComboBox;

namespace VTX::Tool::Mdprep::Gateway
{
	struct EngineSpecificCommonInformation;
	struct GromacsData;
} // namespace VTX::Tool::Mdprep::Gateway
namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;
	struct FormLayouts;
	class MdEngineSpecificFieldPlacer;
	enum class E_FIELD_SECTION;
	// Class responsible for managing ui tied to gromacs
	class GromacsFieldPlacer
	{
	  public:
		GromacsFieldPlacer( FormLayouts );

		void get( MdEngineSpecificFieldPlacer &, const E_FIELD_SECTION & ) noexcept;
		void get( const EngineSpecificCommonInformation *& ) const noexcept;

		void activate() noexcept;
		void deactivate() noexcept;

	  private:
		EngineSpecificCommonInformation _commonData;
		GromacsData					 _specificData;
		FormLayouts					 _layouts;

		bool			 _activatedOnce	  = false;
		QDoubleSpinBox * _fieldConc		  = nullptr;
		QComboBox *		 _fieldWaterModel = nullptr;
		QComboBox *		 _fieldBoxShape	  = nullptr;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
