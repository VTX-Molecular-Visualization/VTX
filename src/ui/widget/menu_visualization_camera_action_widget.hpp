#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_CAMERA_ACTION__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_CAMERA_ACTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "menu_toolblock_widget.hpp"
#include "menu_toolbutton_widget.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class MenuVisualizationCameraActionWidget : public MenuToolBlockWidget
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				~MenuVisualizationCameraActionWidget();
				void localize() override;

			  protected:
				MenuVisualizationCameraActionWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};
				void _setupUi( const QString & p_name ) override;
				void _setupSlots() override;

			  private:
				// Selection focus
				MenuToolButtonWidget * _center	 = nullptr;
				MenuToolButtonWidget * _focus	 = nullptr;
				MenuToolButtonWidget * _reorient = nullptr;

				// Camera Mode
				MenuToolButtonWidget * _trackball = nullptr;
				MenuToolButtonWidget * _freefly	  = nullptr;
				MenuToolButtonWidget * _vessel	  = nullptr;

				// Viewpoints
				MenuToolButtonWidget * _createViewpoint = nullptr;
				MenuToolButtonWidget * _takeScreenshot	= nullptr;

				void _setTrackballController();
				void _setFreeflyController();
				void _setVesselController();
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
