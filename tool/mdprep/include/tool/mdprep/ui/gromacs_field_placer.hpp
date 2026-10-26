#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_GROMACS_FIELD_PLACER__
#define __VTX_TOOL_TOOLS_MDPREP_UI_GROMACS_FIELD_PLACER__

#include <array>
#include <optional>

class QDoubleSpinBox;
class QComboBox;
class QWidget;

namespace VTX::UI::QT::Util
{
	class ObjectOwnership;
}

namespace VTX::Tool::Mdprep::Gateway::Gromacs
{
	struct MdSettings;
}

namespace VTX::Tool::Mdprep::Gateway
{
	struct EngineSpecificCommonInformation;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;
	struct FormLayouts;
	class GromacsSystemFieldPlacer;
	class GromacsMdStepFieldPlacer;

	// Class responsible for managing ui tied to gromacs
	class GromacsFieldPlacer
	{
	  public:
		GromacsFieldPlacer( Gateway::Gromacs::MdSettings & );

		void assign( FormLayouts p_layouts ) noexcept;
		void activate();
		void deactivate();

	  private:
		std::array<VTX::UI::QT::Util::ObjectOwnership, 2> _toDeleteObjects;
		Gateway::Gromacs::MdSettings *					  _specificData;
		std::optional<FormLayouts>						  _layouts;

		struct UiObjects
		{
			QWidget *		 _labelConc = nullptr;
			QDoubleSpinBox * _fieldConc = nullptr;
		} _uiObjects;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
