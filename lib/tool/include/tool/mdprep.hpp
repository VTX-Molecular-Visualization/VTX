#ifndef __VTX_TOOL_MDPREP_REGISTRATION__
#define __VTX_TOOL_MDPREP_REGISTRATION__

#include "ui/core/tool_registry.hpp"
#include "ui/qt/base_qt_widget.hpp"

namespace VTX::Tool
{
	// Class responsible for showing a button that call the MDprep tool
	class ToolMdprep : public VTX::UI::QT::BaseQtTool
	{
	  public:
		inline static const UI::Core::ToolIdentifier TOOL_KEY = "TOOL_MDPREP";

	  private:
		inline static const VTX::UI::Core::ToolRegistry::Registration<ToolMdprep> _reg { TOOL_KEY };

	  public:
		ToolMdprep();

		void instantiateTool() override;

	  private:
		void _addButtonsInMenu();
		void _openMdPrepWindow();

		struct Data;
		struct Del
		{
			void operator()( Data * ) const noexcept;
		};
		std::unique_ptr<Data, Del> _data = nullptr;
	};
} // namespace VTX::Tool

#endif //! __VTX_TOOL_MDPREP_REGISTRATION__
