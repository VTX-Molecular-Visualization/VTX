#ifndef __VTX_UI_WIDGET_SETTING_REPRESENTATION__
#define __VTX_UI_WIDGET_SETTING_REPRESENTATION__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/view/ui/widget/representation/representation_library_view.hpp"
#include <QString>
#include <QWidget>

namespace VTX::UI::Widget::Settings
{
	class SettingRepresentationWidget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET

		using RepresentationLibraryView = View::UI::Widget::Representation::RepresentationLibraryView;

	  public:
		void receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event ) override;
		void localize() override;

	  protected:
		SettingRepresentationWidget( QWidget * const );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		RepresentationLibraryView * _presetLibrary;
	};
} // namespace VTX::UI::Widget::Settings
#endif
