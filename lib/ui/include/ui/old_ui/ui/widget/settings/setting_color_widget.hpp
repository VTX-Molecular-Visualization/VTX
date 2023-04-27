#ifndef __VTX_UI_WIDGET_SETTING_COLOR__
#define __VTX_UI_WIDGET_SETTING_COLOR__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::Settings
{
	class SettingColorWidget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET

	  public:
		void receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event ) override;
		void localize() override;

	  protected:
		SettingColorWidget( QWidget * const );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
	};
} // namespace VTX::UI::Widget::Settings
#endif
