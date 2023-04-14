#ifndef __VTX_UI_WIDGET_MENU_CAMERA_VIEWPOINT_BLOCK__
#define __VTX_UI_WIDGET_MENU_CAMERA_VIEWPOINT_BLOCK__

#include <app/old_app/event/event.hpp>
#include "old_ui/ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "old_ui/ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Camera
{
	class ViewpointBlock : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;

		void receiveEvent( const VTX::Event::VTXEvent & p_event ) override;

	  protected:
		ViewpointBlock( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _enableDeleteButtonState( const bool p_enable );

	  private:
		MenuToolButtonWidget * _createViewpointButton = nullptr;
		MenuToolButtonWidget * _deleteViewpointButton = nullptr;

		void _createViewpointAction() const;
		void _deleteViewpointAction() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Camera
#endif
