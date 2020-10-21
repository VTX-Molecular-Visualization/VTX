#include "menu_visualization_render_effects_widget.hpp"
#include "ui/widget_factory.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			MenuVisualizationRenderEffectsWidget::~MenuVisualizationRenderEffectsWidget() {}

			void MenuVisualizationRenderEffectsWidget::_setupUi( const QString & p_name )
			{
				MenuToolBlockWidget::_setupUi( p_name );

				// Predefined presets
				_predefinedPreset1 = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "applyRenderPresetButton" );
				_predefinedPreset1->setData( "Preset 1", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_predefinedPreset1, 0 );

				_predefinedPreset2 = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "applyRenderPresetButton" );
				_predefinedPreset2->setData( "Preset 2", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_predefinedPreset2, 0 );

				_predefinedPreset3 = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "applyRenderPresetButton" );
				_predefinedPreset3->setData( "Preset 3", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_predefinedPreset3, 0 );

				// Custom presets
				_customPreset1 = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "applyRenderPresetButton" );
				_customPreset1->setData( "Custom Preset 1", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_customPreset1, 1 );

				_customPreset2 = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "applyRenderPresetButton" );
				_customPreset2->setData( "Custom Preset 2", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_customPreset2, 1 );

				_customPreset3 = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "applyRenderPresetButton" );
				_customPreset3->setData( "Custom Preset 3", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_customPreset3, 1 );

				// Camera Mode
				_createPreset = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "createRenderPresetButton" );
				_createPreset->setData( "Add Preset", ":/sprite/new_session_icon.png", Qt::Orientation::Vertical );
				pushButton( *_createPreset, 2 );

				// Camera Mode
				_fullscreen = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "toggleFullscreenButton" );
				_fullscreen->setData( "Fullscreen", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_fullscreen, 2 );

				_capture = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "screenshotButton" );
				_capture->setData( "Capture", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_capture, 2 );

				validate();
			}
			void MenuVisualizationRenderEffectsWidget::_setupSlots() {}
			void MenuVisualizationRenderEffectsWidget::localize() { setTitle( "Render Effects" ); }
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
