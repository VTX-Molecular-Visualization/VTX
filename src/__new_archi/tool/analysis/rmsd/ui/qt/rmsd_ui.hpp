#ifndef __VTX_TOOL_ANALYSIS_RMSD_UI_RMSD_TOOL__
#define __VTX_TOOL_ANALYSIS_RMSD_UI_RMSD_TOOL__

#include "__new_archi/tool/analysis/rmsd/tool_info.hpp"
#include "__new_archi/ui/core/tool_registration.hpp"

namespace VTX::Tool::Analysis::RMSD::UI::QT
{
	class RMSDTool : public VTX::UI::Core::BaseVTXUITool
	{
		REGISTER_TOOL( RMSDTool, TOOL_KEY );

	  public:
		RMSDTool();

		void instantiateTool() override;
	};
} // namespace VTX::Tool::Analysis::RMSD::UI::QT

#endif
