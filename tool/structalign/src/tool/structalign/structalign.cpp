#include <tool/structalign/structalign.hpp>
#include <ui/qt/action_registry.hpp>
#include <ui/qt/services.hpp>
#include <ui/qt/widget/main_window.hpp>

namespace VTX::Tool::Structalign
{
	constexpr std::string_view ACTION_OPEN_STRUCTALIGN = "tool.structalign.open";

	namespace
	{
		App::UI::DescAction openStructAlignAction()
		{
			App::UI::DescAction action;
			action.key		= ACTION_OPEN_STRUCTALIGN;
			action.name		= "StructAlign";
			action.tip		= "Align molecular systems";
			action.icon		= "sprite/structural_alignment_icon.png";
			action.shortcut = "ctrl+alt+A";
			return action;
		}
	} // namespace

	StructAlign::StructAlign() = default;

	void StructAlign::registerActions()
	{
		UI::QT::UI_ACTIONS().registerAction(
			openStructAlignAction(), []( const UI::QT::ActionRegistry::ActionContext & ) { openStructAlignAction(); }
		);
	}

	void StructAlign::buildUI()
	{
		auto & mainWindow = UI::QT::MAIN_WINDOW();
		mainWindow.addMenuAction( "Tool", ACTION_OPEN_STRUCTALIGN );
		mainWindow.addToolBarAction( "Tool", ACTION_OPEN_STRUCTALIGN );
	}
} // namespace VTX::Tool::Structalign
