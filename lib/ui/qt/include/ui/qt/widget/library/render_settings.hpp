#ifndef __VTX_UI_QT_WIDGET_LIBRARY_RENDER_SETTINGS__
#define __VTX_UI_QT_WIDGET_LIBRARY_RENDER_SETTINGS__

#include "base_preset_widget.hpp"
#include "ui/qt/core/widget/hideable_goupe_box.hpp"
#include <app/action/render_settings.hpp>
#include <app/library/preset/render_settings.hpp>

namespace VTX::UI::QT::Widget::Library
{
	class RenderSettings final : public BasePresetWidget<App::Library::Preset::RenderSettings>
	{
	  public:
		RenderSettings( QWidget * p_parent );

	  protected:
		void _onPresetAdded( const std::string_view ) override;
		void _onPresetChanged() override;
	};
} // namespace VTX::UI::QT::Widget::Library

#endif
