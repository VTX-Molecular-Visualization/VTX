#include "ui/qt/widget/library/render_settings.hpp"

namespace VTX::UI::QT::Widget::Library
{
	RenderSettings::RenderSettings( QWidget * p_parent ) : BasePresetWidget( p_parent ) {}

	void RenderSettings::_onPresetAdded( const std::string_view p_name ) {}

	void RenderSettings::_onPresetChanged() {}
} // namespace VTX::UI::QT::Widget::Library
