#ifndef __VTX_UI_WIDGET_MENU_CAMERA_PROJECTION__
#define __VTX_UI_WIDGET_MENU_CAMERA_PROJECTION__

#include "event/event.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Camera
{
	class CameraProjectionBlock : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~CameraProjectionBlock();
		void localize() override;

		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		CameraProjectionBlock( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		// Camera projection
		MenuToolButtonWidget * _cameraProjectionButton = nullptr;

		void _refreshCameraProjectionButton() const;
		void _toggleCameraProjection() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Camera
#endif
