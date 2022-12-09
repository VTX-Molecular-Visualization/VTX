#ifndef __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_TOOL__
#define __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_TOOL__

#include "__new_archi/tool/analysis/structural_alignment/tool_info.hpp"
#include "__new_archi/ui/core/tool_registration.hpp"
#include "ui/widget/analysis/structural_alignment/structural_alignment_widget.hpp"

namespace VTX::Tool::Analysis::StructuralAlignment::UI::QT
{
	class StructuralAlignmentTool : public VTX::UI::Core::BaseVTXUITool
	{
		REGISTER_TOOL( StructuralAlignmentTool, TOOL_KEY );

		using StructuralAlignmentModelListWidget
			= VTX::UI::Widget::Analysis::StructuralAlignment::StructuralAlignmentModelListWidget;

	  public:
		StructuralAlignmentTool();

		void instantiateTool() override;
	};
} // namespace VTX::Tool::Analysis::StructuralAlignment::UI::QT

#endif
