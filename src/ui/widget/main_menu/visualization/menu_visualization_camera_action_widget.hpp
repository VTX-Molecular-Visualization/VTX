#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_CAMERA_ACTION__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_CAMERA_ACTION__

#include "event/event.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Visualization
{
	class MenuVisualizationCameraActionWidget : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~MenuVisualizationCameraActionWidget();
		void localize() override;

		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		MenuVisualizationCameraActionWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
		{
			_registerEvent( Event::Global::CONTROLLER_CHANGE );
		};
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		// Selection focus
		MenuToolButtonWidget * _center	 = nullptr;
		MenuToolButtonWidget * _reorient = nullptr;

		// Camera Mode
		MenuToolButtonWidget * _trackball = nullptr;
		MenuToolButtonWidget * _freefly	  = nullptr;
		// !V0.1
		// MenuToolButtonWidget * _vessel	  = nullptr;

		// Viewpoints
		// !V0.1
		// MenuToolButtonWidget * _createViewpoint = nullptr;

		void _updateCameraModeFeedback();

		void _recenterCamera() const;
		void _orientCamera() const;
		void _setTrackballController() const;
		void _setFreeflyController() const;
		void _setVesselController() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Visualization
#endif
