#ifndef __VTX_UI_QT_WIDGET_TREE_REPRESENTATION_PRESETS__
#define __VTX_UI_QT_WIDGET_TREE_REPRESENTATION_PRESETS__

#include "ui/qt/widget/tree/base_tree_presets.hpp"
#include <renderer/representation.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	class RepresentationPresets : public Widget::Tree::BaseTreePreset<Renderer::Representation>
	{
	  public:
		RepresentationPresets( QWidget * p_parent ) : BaseTreePreset( p_parent )
		{
			addTopLevelItem( new QTreeWidgetItem( QStringList() << "Representations" ) );
			topLevelItem( 0 )->setIcon( 0, STYLE().iconFromCodepoint( Style::Icons::REPRESENTATION ) );
		}
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
