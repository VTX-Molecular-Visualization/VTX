#include "app/action/visualization.hpp"
#include "app/core/controller/base_camera_controller.hpp"
#include "app/core/controller/controller_manager.hpp"
#include "app/mode/base_mode.hpp"
#include "app/mode/visualization.hpp"
#include <util/collection.hpp>

namespace VTX::App::Action::Visualization
{
	void ToggleCameraController ::execute()
	{
		// if ( QT::MODE().getName() != QT::Mode::Visualization::ID )
		//	return;
		using namespace App;
		using namespace App::Core;

		Mode::Visualization & visualizationMode = dynamic_cast<Mode::Visualization &>( App::MODE() );

		const App::Core::Controller::BaseCameraController & currentCameraController
			= visualizationMode.getCurrentCameraController();

		const Util::Hashing::Hash & controllerHashKey = currentCameraController.getHashedCollectionID();

		const std::set<std::unique_ptr<Controller::BaseController>> & controllerSet
			= visualizationMode.getCameraControllers();

		auto it = std::find_if(
			controllerSet.begin(),
			controllerSet.end(),
			[ controllerHashKey ]( const std::unique_ptr<Controller::BaseController> & controllerPtr )
			{ return controllerPtr.get()->getHashedCollectionID() == controllerHashKey; }
		);

		if ( ++it != controllerSet.end() )
		{
			visualizationMode.setCameraController( ( *it )->getHashedCollectionID() );
		}
		else
		{
			visualizationMode.setCameraController( ( *controllerSet.begin() )->getHashedCollectionID() );
		}
	}

	void ChangeCameraController::execute()
	{
		// if ( QT::MODE().getName() != QT::Mode::Visualization::ID )
		//	return;

		App::Mode::Visualization & visualizationMode = dynamic_cast<App::Mode::Visualization &>( App::MODE() );
		visualizationMode.setCameraController( _controllerID );
	}

	void ApplyNextCameraController::execute()
	{
		// if ( QT::MODE().getName() != QT::Mode::Visualization::ID )
		//	return;

		App::Mode::Visualization & visualizationMode = dynamic_cast<App::Mode::Visualization &>( App::MODE() );
		visualizationMode.getCurrentCameraController();
	}
} // namespace VTX::App::Action::Visualization
