#include "widget/ui_features/visualization.hpp"
#include "application_qt.hpp"
#include "core/main_menu/menu_toolblock_widget.hpp"
#include "core/main_menu/menu_toolbutton_widget.hpp"
#include "main_window.hpp"
#include "widget/pytx/include_python_binding.hpp"
#include "widget_factory.hpp"
#include <app/action/animation.hpp>
#include <app/action/visualization.hpp>
#include <app/application/scene.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/application/system/action_manager.hpp>
#include <app/controller/camera/freefly.hpp>
#include <app/controller/camera/trackball.hpp>
#include <app/core/action/base_action.hpp>
#include <app/mode/visualization.hpp>
#include <app/vtx_app.hpp>
#include <string>

namespace VTX::UI::QT::Widget::UIFeatures
{
	VisualizationTool::VisualizationTool()
	{
		_addButtonsInMainMenu();
		_addCommands();
	}

	void VisualizationTool::_addButtonsInMainMenu()
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		UI::Core::ToolLayoutData navigationLayout = UI::Core::ToolLayoutData();
		navigationLayout.tabName				  = "Visualization";
		navigationLayout.blockName				  = "Navigation";

		QT::Core::MainMenu::MenuToolBlockWidget & toolBlock = mainWindow->getMainMenuToolBlock( navigationLayout );

		QT::Core::MainMenu::MenuToolButtonWidget * const resetCameraButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Core::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "resetCameraButton"
			);
		resetCameraButton->setData( "Reset", ":/sprite/camera_recenter_icon.png", Qt::Orientation::Horizontal );
		resetCameraButton->setTriggerAction( this, &VisualizationTool::_resetCamera );

		QT::Core::MainMenu::MenuToolButtonWidget * const orientCameraButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Core::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "orientCameraButton"
			);
		orientCameraButton->setData( "Orient", ":/sprite/camera_orient_icon.png", Qt::Orientation::Horizontal );
		orientCameraButton->setTriggerAction( this, &VisualizationTool::_orientCamera );

		QT::Core::MainMenu::MenuToolButtonWidget * const trackballButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Core::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "trackballButton"
			);
		trackballButton->setData( "Trackball", ":/sprite/camera_trackball_mode.png", Qt::Orientation::Horizontal );
		trackballButton->setTriggerAction( this, &VisualizationTool::_setTrackball );

		QT::Core::MainMenu::MenuToolButtonWidget * const freeflyButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Core::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "freeflyButton"
			);
		freeflyButton->setData( "Freefly", ":/sprite/camera_freefly_mode.png", Qt::Orientation::Horizontal );
		freeflyButton->setTriggerAction( this, &VisualizationTool::_setFreefly );

		toolBlock.pushButton( *resetCameraButton, *orientCameraButton );
		toolBlock.pushButton( *trackballButton, *freeflyButton );
	}

	void VisualizationTool::_resetCamera() const { App::VTX_ACTION().execute<App::Action::Animation::ResetCamera>(); }
	void VisualizationTool::_orientCamera() const
	{
		if ( App::CURRENT_SELECTION().isEmpty() )
			App::VTX_ACTION().execute<App::Action::Animation::Orient>( App::SCENE().getAABB() );
		else
			App::VTX_ACTION().execute<App::Action::Animation::Orient>( App::CURRENT_SELECTION().getAABB() );
	}

	void VisualizationTool::_setTrackball() const
	{
		App::VTX_ACTION().execute<App::Action::Visualization::ChangeCameraController>(
			App::Controller::Camera::Trackball::COLLECTION_ID
		);
	}
	void VisualizationTool::_setFreefly() const
	{
		App::VTX_ACTION().execute<App::Action::Visualization::ChangeCameraController>(
			App::Controller::Camera::Freefly::COLLECTION_ID
		);
	}

	namespace Visualization
	{
		class Binder final : public PythonBinding::Binder
		{
		  public:
			void bind( PythonBinding::PyTXModule & p_vtxmodule ) override
			{
				PythonBinding::Wrapper::Module & commands = p_vtxmodule.commands();

				commands.bindAction<App::Action::Animation::ResetCamera>(
					"reset_camera", "Reset the camera position and orientation."
				);
				commands.bindAction<App::Action::Visualization::ChangeCameraController, std::string>(
					"set_camera_controller", "Set the camera controller."
				);
			}
		};
	} // namespace Visualization

	void VisualizationTool::_addCommands() const { PythonBinding::INTERPRETOR().addBinder<Visualization::Binder>(); }

} // namespace VTX::UI::QT::Widget::UIFeatures
