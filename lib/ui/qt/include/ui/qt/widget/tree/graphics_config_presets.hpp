#ifndef __VTX_UI_QT_WIDGET_TREE_GRAPHICS_CONFIG_PRESETS__
#define __VTX_UI_QT_WIDGET_TREE_GRAPHICS_CONFIG_PRESETS__

#include "ui/qt/widget/tree/base_tree_presets.hpp"
#include <renderer/graphics_config.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	class GraphicsConfigPresets : public Widget::Tree::BaseTreePreset<Renderer::GraphicsConfig>
	{
	  public:
		GraphicsConfigPresets( QWidget * p_parent ) : BaseTreePreset( p_parent )
		{
			addTopLevelItem( new QTreeWidgetItem( QStringList() << "Graphics" ) );
		}
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
