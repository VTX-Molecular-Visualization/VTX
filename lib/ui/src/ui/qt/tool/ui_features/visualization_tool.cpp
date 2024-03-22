#include "ui/qt/tool/ui_features/visualization_tool.hpp"
#include "ui/action/animation.hpp"
#include "ui/action/visualization.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/controller/freefly.hpp"
#include "ui/qt/controller/trackball.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/mode/visualization.hpp"
#include "ui/qt/tool/pytx/details/include_python_binding.hpp"
#include "ui/qt/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include "ui/qt/widget_factory.hpp"
#include <app/application/scene.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/application/system/action_manager.hpp>
#include <app/core/action/base_action.hpp>
#include <app/vtx_app.hpp>
#include <string>

namespace VTX::UI::QT::Tool::UIFeatures
{
	VisualizationTool::VisualizationTool() : BaseQtTool() {}

	void VisualizationTool::instantiateTool()
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

		QT::Widget::MainMenu::MenuToolBlockWidget & toolBlock = mainWindow->getMainMenuToolBlock( navigationLayout );

		QT::Widget::MainMenu::MenuToolButtonWidget * const resetCameraButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "resetCameraButton"
			);
		resetCameraButton->setData( "Reset", ":/sprite/camera_recenter_icon.png", Qt::Orientation::Horizontal );
		resetCameraButton->setTriggerAction( this, &VisualizationTool::_resetCamera );

		QT::Widget::MainMenu::MenuToolButtonWidget * const orientCameraButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "orientCameraButton"
			);
		orientCameraButton->setData( "Orient", ":/sprite/camera_orient_icon.png", Qt::Orientation::Horizontal );
		orientCameraButton->setTriggerAction( this, &VisualizationTool::_orientCamera );

		QT::Widget::MainMenu::MenuToolButtonWidget * const trackballButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "trackballButton"
			);
		trackballButton->setData( "Trackball", ":/sprite/camera_trackball_mode.png", Qt::Orientation::Horizontal );
		trackballButton->setTriggerAction( this, &VisualizationTool::_setTrackball );

		QT::Widget::MainMenu::MenuToolButtonWidget * const freeflyButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "freeflyButton"
			);
		freeflyButton->setData( "Freefly", ":/sprite/camera_freefly_mode.png", Qt::Orientation::Horizontal );
		freeflyButton->setTriggerAction( this, &VisualizationTool::_setFreefly );

		toolBlock.pushButton( *resetCameraButton, *orientCameraButton );
		toolBlock.pushButton( *trackballButton, *freeflyButton );
	}

	void VisualizationTool::_resetCamera() const { App::VTX_ACTION().execute<Action::Animation::ResetCamera>(); }
	void VisualizationTool::_orientCamera() const
	{
		if ( App::CURRENT_SELECTION().isEmpty() )
			App::VTX_ACTION().execute<Action::Animation::Orient>( App::SCENE().getAABB() );
		else
			App::VTX_ACTION().execute<Action::Animation::Orient>( App::CURRENT_SELECTION().getAABB() );
	}

	void VisualizationTool::_setTrackball() const
	{
		App::VTX_ACTION().execute<Action::Visualization::ChangeCameraController>( Controller::Trackball::COLLECTION_ID
		);
	}
	void VisualizationTool::_setFreefly() const
	{
		App::VTX_ACTION().execute<Action::Visualization::ChangeCameraController>( Controller::Freefly::COLLECTION_ID );
	}

	namespace Visualization
	{
		class Binder final : public PythonBinding::Binder
		{
		  public:
			void bind( PythonBinding::PyTXModule & p_vtxmodule ) override
			{
				PythonBinding::Wrapper::Module & commands = p_vtxmodule.commands();

				commands.bindAction<Action::Animation::ResetCamera>(
					"reset_camera", "Reset the camera position and orientation."
				);
				commands.bindAction<Action::Visualization::ChangeCameraController, std::string>(
					"set_camera_controller", "Set the camera controller."
				);
			}
		};
	} // namespace Visualization

	void VisualizationTool::_addCommands() const { PythonBinding::INTERPRETOR().addBinder<Visualization::Binder>(); }

} // namespace VTX::UI::QT::Tool::UIFeatures
