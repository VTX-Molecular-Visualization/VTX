#ifndef __VTX_UI_QT_WIDGET_TREE_COLOR_LAYOUT_PRESETS__
#define __VTX_UI_QT_WIDGET_TREE_COLOR_LAYOUT_PRESETS__

#include "ui/qt/widget/tree/base_tree_presets.hpp"
#include <renderer/color.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	class ColorLayoutPresets : public Widget::Tree::BaseTreePreset<VTX::Renderer::Color::Layout>
	{
	  public:
		ColorLayoutPresets( QWidget * p_parent ) : BaseTreePreset<VTX::Renderer::Color::Layout>( p_parent )
		{
			addTopLevelItem( new QTreeWidgetItem( QStringList() << "Color layouts" ) );
			topLevelItem( 0 )->setIcon( 0, STYLE().iconFromCodepoint( Style::Icons::COLOR_LAYOUT ) );
			_restoreExpansionState( SETTING_KEY_COLOR_LAYOUT_PRESETS_EXPANDED );
		}
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
