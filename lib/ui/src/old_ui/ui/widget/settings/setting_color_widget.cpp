#include "ui/old_ui/ui/widget/settings/setting_color_widget.hpp"

namespace VTX::UI::Widget::Settings
{
	SettingColorWidget::SettingColorWidget( QWidget * const p_parent ) : BaseManualWidget( p_parent ) {}

	void SettingColorWidget::receiveEvent( const VTX::Event::VTXEvent & p_event ) {}

	void SettingColorWidget::_setupUi( const QString & p_name ) {}

	void SettingColorWidget::_setupSlots() {};
	void SettingColorWidget::localize() {};

} // namespace VTX::UI::Widget::Settings
