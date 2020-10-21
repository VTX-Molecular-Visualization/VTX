#include "menu_visualization_camera_action_widget.hpp"
#include "ui/widget_factory.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			MenuVisualizationCameraActionWidget::~MenuVisualizationCameraActionWidget() {}

			void MenuVisualizationCameraActionWidget::_setupUi( const QString & p_name )
			{
				MenuToolBlockWidget::_setupUi( p_name );

				// Selection focus
				_center = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
				_center->setData( "Center", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_center, 0 );

				_focus = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
				_focus->setData( "Focus", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_focus, 0 );

				_reorient = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
				_reorient->setData( "Orient", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_reorient, 0 );

				// Camera Mode
				_trackball = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
				_trackball->setData( "Trackball", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_trackball, 1 );

				_freefly = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
				_freefly->setData( "Freecam", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_freefly, 1 );

				_vessel = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
				_vessel->setData( "Vessel", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_vessel, 1 );

				// Viewpoints
				_createViewpoint = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
				_createViewpoint->setData( "Add\nViewpoint", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_createViewpoint, 2 );

				_takeScreenshot = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "centerCameraOnSelectionButton" );
				_takeScreenshot->setData( "Capture", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_takeScreenshot, 2 );

				validate();
			}

			void MenuVisualizationCameraActionWidget::localize() { setTitle( "Camera Action" ); }

			void MenuVisualizationCameraActionWidget::_setupSlots() {}
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
