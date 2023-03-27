#ifndef __VTX_UI_WIDGET_MENU_NAVIGATION_ACTION__
#define __VTX_UI_WIDGET_MENU_NAVIGATION_ACTION__

#include "event/event.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Camera
{
	class CameraNavigationActionBlock : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~CameraNavigationActionBlock();
		void localize() override;

		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		CameraNavigationActionBlock( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		// Selection focus
		MenuToolButtonWidget * _center	 = nullptr;
		MenuToolButtonWidget * _reorient = nullptr;

		// Camera Controller
		MenuToolButtonWidget * _trackball = nullptr;
		MenuToolButtonWidget * _freefly	  = nullptr;
		// !V0.1
		// MenuToolButtonWidget * _vessel	  = nullptr;

		// Viewpoints
		// !V0.1
		// MenuToolButtonWidget * _createViewpoint = nullptr;

		void _updateCameraModeFeedback();

		void _toggleCameraProjection() const;
		void _recenterCamera() const;
		void _orientCamera() const;
		void _setTrackballController() const;
		void _setFreeflyController() const;
		void _setVesselController() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Camera
#endif
