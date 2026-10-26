#ifndef __VTX_UI_QT_WIDGET_TREE_GRAPHICS_CONFIG_PRESETS__
#define __VTX_UI_QT_WIDGET_TREE_GRAPHICS_CONFIG_PRESETS__

#include "ui/qt/widget/tree/base_tree_presets.hpp"
#include <app/scene/graphics_config.hpp>
#include <renderer/graphics_config.hpp>

namespace VTX::UI::QT::Widget::Tree
{

	class GraphicsConfigPresets : public Widget::Tree::BaseTreePreset<VTX::Renderer::GraphicsConfig>
	{
	  public:
		GraphicsConfigPresets( QWidget * const p_parent ) : BaseTreePreset<VTX::Renderer::GraphicsConfig>( p_parent )
		{
			addTopLevelItem( new QTreeWidgetItem( QStringList() << "Graphics" ) );
			topLevelItem( 0 )->setIcon( 0, STYLE().iconFromCodepoint( Style::Icons::GRAPHICS_CONFIG ) );
			_restoreExpansionState( SETTING_KEY_GRAPHICS_CONFIG_PRESETS_EXPANDED );
			_connectAppliedPresetUpdates( App::REG() );
		}

	  private:
		void _connectAppliedPresetUpdates( Registry & p_reg )
		{
			_appliedPresetConstructedConnection = p_reg.on_construct<App::Scene::GraphicsConfig>()
													  .connect<&GraphicsConfigPresets::_onSceneGraphicsConfig>( this );
			_appliedPresetUpdatedConnection
				= p_reg.on_update<App::Scene::GraphicsConfig>().connect<&GraphicsConfigPresets::_onSceneGraphicsConfig>(
					this
				);
			_restoreCurrentSceneGraphicsConfig( p_reg );
		}

		void _restoreCurrentSceneGraphicsConfig( Registry & p_reg )
		{
			auto view = p_reg.view<App::Scene::GraphicsConfig>();
			if ( view.begin() != view.end() )
			{
				_setAppliedPreset( p_reg.get<App::Scene::GraphicsConfig>( *view.begin() ).preset );
			}
		}

		void _onSceneGraphicsConfig( Registry & p_reg, const Entity p_e )
		{ _setAppliedPreset( p_reg.get<App::Scene::GraphicsConfig>( p_e ).preset ); }

		VTX::Util::EventHub::ScopedConnection _appliedPresetConstructedConnection;
		VTX::Util::EventHub::ScopedConnection _appliedPresetUpdatedConnection;
	};

} // namespace VTX::UI::QT::Widget::Tree

#endif
