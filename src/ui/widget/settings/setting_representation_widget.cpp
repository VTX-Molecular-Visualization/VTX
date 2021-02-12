#include "setting_representation_widget.hpp"
#include "style.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::Settings
{
	SettingRepresentationWidget::SettingRepresentationWidget( QWidget * const p_parent ) : BaseManualWidget( p_parent )
	{
	}

	void SettingRepresentationWidget::receiveEvent( const Event::VTXEvent & p_event ) {}

	void SettingRepresentationWidget::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }

	void SettingRepresentationWidget::_setupSlots() {};
	void SettingRepresentationWidget::localize() {};

} // namespace VTX::UI::Widget::Settings
