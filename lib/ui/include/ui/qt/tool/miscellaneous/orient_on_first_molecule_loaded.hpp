#ifndef __VTX_UI_QT_TOOL_MISCELLANEOUS_ORIENT_ON_FIRST_MOLECULE_LOADED__
#define __VTX_UI_QT_TOOL_MISCELLANEOUS_ORIENT_ON_FIRST_MOLECULE_LOADED__

#include "ui/core/tool_registration.hpp"
#include "ui/qt/base_qt_tool.hpp"
#include <app/component/scene/scene_item_component.hpp>

namespace VTX::UI::QT::Tool::Miscellaneous
{
	class OrientOnFirstMoleculeLoadedTool : public BaseQtTool
	{
		REGISTER_TOOL( OrientOnFirstMoleculeLoadedTool, "MISCELLANEOUS_ORIENT_ON_FIRST_MOLECULE_LOADED" );

	  public:
		OrientOnFirstMoleculeLoadedTool();
		void instantiateTool() override;

	  private:
		void _onItemAddedInScene( const App::Component::Scene::SceneItemComponent & p_item ) const;
	};
} // namespace VTX::UI::QT::Tool::Miscellaneous

#endif
