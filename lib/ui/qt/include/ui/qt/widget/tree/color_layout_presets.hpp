#ifndef __VTX_UI_QT_WIDGET_TREE_COLOR_LAYOUT_PRESETS__
#define __VTX_UI_QT_WIDGET_TREE_COLOR_LAYOUT_PRESETS__

#include "ui/qt/widget/tree/base_tree_presets.hpp"
#include <app/scene/color_layout.hpp>
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
			_connectAppliedPresetUpdates( App::REG() );
		}

		~ColorLayoutPresets() { _disconnectAppliedPresetUpdates( App::REG() ); }

	  private:
		void _connectAppliedPresetUpdates( Registry & p_reg )
		{
			p_reg.on_construct<App::Scene::ColorLayout>().connect<&ColorLayoutPresets::_onSceneColorLayout>( this );
			p_reg.on_update<App::Scene::ColorLayout>().connect<&ColorLayoutPresets::_onSceneColorLayout>( this );
			_restoreCurrentSceneColorLayout( p_reg );
		}

		void _disconnectAppliedPresetUpdates( Registry & p_reg )
		{
			p_reg.on_construct<App::Scene::ColorLayout>().disconnect<&ColorLayoutPresets::_onSceneColorLayout>( this );
			p_reg.on_update<App::Scene::ColorLayout>().disconnect<&ColorLayoutPresets::_onSceneColorLayout>( this );
		}

		void _restoreCurrentSceneColorLayout( Registry & p_reg )
		{
			auto view = p_reg.view<App::Scene::ColorLayout>();
			if ( view.begin() != view.end() )
			{
				_setAppliedPreset( p_reg.get<App::Scene::ColorLayout>( *view.begin() ).preset );
			}
		}

		void _onSceneColorLayout( Registry & p_reg, const Entity p_e )
		{ _setAppliedPreset( p_reg.get<App::Scene::ColorLayout>( p_e ).preset ); }
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
