#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_GROMACS_FIELD_PLACER__
#define __VTX_TOOL_TOOLS_MDPREP_UI_GROMACS_FIELD_PLACER__

#include <optional>

class QDoubleSpinBox;
class QComboBox;
class QWidget;

namespace VTX::Tool::Mdprep::Gateway
{
	struct EngineSpecificCommonInformation;
	struct GromacsData;
} // namespace VTX::Tool::Mdprep::Gateway
namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;
	struct FormLayouts;
	class MdEngine;
	class MdEngineSpecificFieldPlacer;
	enum class E_FIELD_SECTION;
	// Class responsible for managing ui tied to gromacs
	class GromacsFieldPlacer
	{
	  public:
		GromacsFieldPlacer( GromacsData & );
		~GromacsFieldPlacer();

		void assign( FormLayouts p_layouts ) noexcept;
		void activate() noexcept;
		void deactivate() noexcept;
		void get( const MdEngine *& p_out ) const noexcept;

	  private:
		GromacsData *			   _specificData;
		std::optional<FormLayouts> _layouts;

		QWidget *		 _labelConc = nullptr;
		QDoubleSpinBox * _fieldConc = nullptr;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
