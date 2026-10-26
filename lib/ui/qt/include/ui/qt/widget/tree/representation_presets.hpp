#ifndef VTX_UI_QT_WIDGET_TREE_REPRESENTATION_PRESETS_HPP
#define VTX_UI_QT_WIDGET_TREE_REPRESENTATION_PRESETS_HPP

#include "ui/qt/widget/tree/base_tree_presets.hpp"
#include <memory>
#include <renderer/representation.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	class RepresentationPresets : public Widget::Tree::BaseTreePreset<VTX::Renderer::Representation>
	{
	  public:
		RepresentationPresets( QWidget * const p_parent ) : BaseTreePreset<VTX::Renderer::Representation>( p_parent )
		{
			addTopLevelItem( std::make_unique<QTreeWidgetItem>( QStringList() << "Representations" ).release() );
			topLevelItem( 0 )->setIcon( 0, STYLE().iconFromCodepoint( Style::Icons::REPRESENTATION ) );
			_restoreExpansionState( SETTING_KEY_REPRESENTATION_PRESETS_EXPANDED );
		}
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
