#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_CAMERA_ACTION__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_CAMERA_ACTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				namespace Visualization
				{
					class MenuVisualizationCameraActionWidget : public MenuToolBlockWidget
					{
						VTX_MANUAL_WIDGET_DECLARATION

					  public:
						~MenuVisualizationCameraActionWidget();
						void localize() override;

						void receiveEvent( const Event::VTXEvent & p_event ) override;

					  protected:
						MenuVisualizationCameraActionWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) { _registerEvent( Event::Global::CONTROLLER_CHANGE ); };
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
						MenuToolButtonWidget * _takeSnapshot	= nullptr;

						void _updateCameraModeFeedback();

						void _recenterCamera() const;
						void _setTrackballController() const;
						void _setFreeflyController() const;
						void _setVesselController() const;
						void _takeSnapshotAction() const;
					};
				} // namespace Visualization
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
