#ifndef __VTX_UI_WIDGET_MENU_TOOL_MEASUREMENT__
#define __VTX_UI_WIDGET_MENU_TOOL_MEASUREMENT__

#include "controller/measurement_picker.hpp"
#include "event/event.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QMenu>
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Tool
{
	class MenuToolMeasurementWidget : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~MenuToolMeasurementWidget();

		void localize() override;
		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		MenuToolMeasurementWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		MenuToolButtonWidget * _distanceModeButton		  = nullptr;
		MenuToolButtonWidget * _distanceToCycleModeButton = nullptr;
		MenuToolButtonWidget * _angleModeButton			  = nullptr;
		MenuToolButtonWidget * _dihedralAngleModeButton	  = nullptr;

		void _applyDistanceModeAction() const;
		void _applyDistanceToCycleModeAction() const;
		void _applyAngleModeAction() const;
		void _applyDihedralAngleModeAction() const;

		void _togglePickerState( const Controller::MeasurementPicker::Mode & p_measurementMode ) const;

		void _refreshButtons() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Tool
#endif
