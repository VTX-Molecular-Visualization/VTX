#ifndef __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_TOOL__
#define __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_TOOL__

#include "analysis/structural_alignment/tool_info.hpp"
#include "qt/base_qt_tool.hpp"
#include <old/ui/widget/analysis/structural_alignment/structural_alignment_widget.hpp>
#include <ui/core/tool_registration.hpp>

namespace VTX::Tool::Analysis::StructuralAlignment::UI::QT
{
	class StructuralAlignmentTool : public VTX::Tool::QT::BaseQtTool
	{
		REGISTER_TOOL( StructuralAlignmentTool, TOOL_KEY );

		using StructuralAlignmentModelListWidget
			= VTX::UI::Widget::Analysis::StructuralAlignment::StructuralAlignmentModelListWidget;

	  public:
		StructuralAlignmentTool();

		void instantiateTool() override;

	  private:
		void _addButtonsInMainMenu() const;
	};
} // namespace VTX::Tool::Analysis::StructuralAlignment::UI::QT

#endif
