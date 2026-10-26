#ifndef VTX_UI_QT_WIDGET_TREE_COLOR_LAYOUT_PRESETS_HPP
#define VTX_UI_QT_WIDGET_TREE_COLOR_LAYOUT_PRESETS_HPP

#include "ui/qt/widget/tree/base_tree_presets.hpp"
#include <app/scene/color_layout.hpp>
#include <memory>
#include <renderer/color.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	class ColorLayoutPresets : public Widget::Tree::BaseTreePreset<VTX::Renderer::Color::Layout>
	{
	  public:
		ColorLayoutPresets( QWidget * const p_parent ) : BaseTreePreset<VTX::Renderer::Color::Layout>( p_parent )
		{
			addTopLevelItem( std::make_unique<QTreeWidgetItem>( QStringList() << "Color layouts" ).release() );
			topLevelItem( 0 )->setIcon( 0, STYLE().iconFromCodepoint( Style::Icons::COLOR_LAYOUT ) );
			_restoreExpansionState( SETTING_KEY_COLOR_LAYOUT_PRESETS_EXPANDED );
			_connectAppliedPresetUpdates( App::REG() );
		}

	  private:
		void _connectAppliedPresetUpdates( Registry & p_reg )
		{
			_appliedPresetConstructedConnection
				= p_reg.on_construct<App::Scene::ColorLayout>().connect<&ColorLayoutPresets::_onSceneColorLayout>(
					this );
			_appliedPresetUpdatedConnection
				= p_reg.on_update<App::Scene::ColorLayout>().connect<&ColorLayoutPresets::_onSceneColorLayout>( this );
			_restoreCurrentSceneColorLayout( p_reg );
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

		VTX::Util::EventHub::ScopedConnection _appliedPresetConstructedConnection;
		VTX::Util::EventHub::ScopedConnection _appliedPresetUpdatedConnection;
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
