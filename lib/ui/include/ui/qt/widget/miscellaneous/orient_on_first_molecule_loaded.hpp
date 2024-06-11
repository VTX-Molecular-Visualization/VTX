#ifndef __VTX_UI_QT_TOOL_MISCELLANEOUS_ORIENT_ON_FIRST_MOLECULE_LOADED__
#define __VTX_UI_QT_TOOL_MISCELLANEOUS_ORIENT_ON_FIRST_MOLECULE_LOADED__

#include "ui/core/tool_registry.hpp"
#include "ui/qt/base_qt_widget.hpp"
#include <app/component/scene/scene_item_component.hpp>

namespace VTX::UI::QT::Widget::Miscellaneous
{
	class OrientOnFirstMoleculeLoadedTool : public BaseQtTool
	{
	  public:
		inline static const UI::Core::ToolIdentifier TOOL_KEY = "MISCELLANEOUS_ORIENT_ON_FIRST_MOLECULE_LOADED";

	  private:
		inline static const UI::Core::ToolRegistry::Registration<OrientOnFirstMoleculeLoadedTool> _reg { TOOL_KEY };

	  public:
		OrientOnFirstMoleculeLoadedTool();
		void instantiateTool() override;

	  private:
		void _onItemAddedInScene( const App::Component::Scene::SceneItemComponent & p_item ) const;
	};
} // namespace VTX::UI::QT::Widget::Miscellaneous

#endif
