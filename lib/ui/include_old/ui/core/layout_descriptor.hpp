#ifndef __VTX_UI_CORE_LAYOUT_DESCRIPTOR__
#define __VTX_UI_CORE_LAYOUT_DESCRIPTOR__

#include <string>
#include <vector>

namespace VTX::UI::Core
{
	struct LayoutDescriptor
	{
		struct Button
		{
			std::string name;

			std::string tabName;
			std::string blockName;

			// TODO: bind action (check python binders)
			// Util::Callback<> callback;
		};
		using Buttons = std::vector<Button>;

		// TODO
		struct Panel
		{
		};
		using Panels = std::vector<Panel>;

		Buttons buttons;
		Panels	panels;
	};

	// using ToolIdentifier									   = std::string;
	// inline static const ToolIdentifier UNKNOWN_TOOL_IDENTIFIER = "";

	/*
	class ToolLayoutData
	{
	  public:
		std::string tabName;
		std::string blockName;

		std::string contextualMenuName;
	};
	*/

	/*
	class ToolDescriptor
	{
	  public:
		ToolDescriptor();
		ToolDescriptor( const ToolIdentifier & p_identifier );
		ToolDescriptor( const ToolIdentifier & p_identifier, const ToolLayoutData p_layoutData );

		const ToolIdentifier & getIdentifier() const { return _identifier; }
		const ToolLayoutData & getLayoutData() const { return _layoutData; }

		void setIdentifier( const ToolIdentifier & p_identifier ) { _identifier = p_identifier; }
		void setToolLayoutData( const ToolLayoutData & p_layoutData ) { _layoutData = p_layoutData; }

	  private:
		ToolIdentifier _identifier;
		ToolLayoutData _layoutData = ToolLayoutData();
	};

	class LayoutDescriptor
	{
	  public:
		void								addTool( const ToolDescriptor & p_toolDescriptor );
		const std::vector<ToolDescriptor> & getTools() const { return _tools; };

	  private:
		std::vector<ToolDescriptor> _tools;
	};
	*/

} // namespace VTX::UI::Core

#endif
