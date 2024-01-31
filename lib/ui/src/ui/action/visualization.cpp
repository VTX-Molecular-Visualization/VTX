#include "ui/action/visualization.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/controller/base_camera_controller.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include "ui/qt/mode/base_mode.hpp"
#include "ui/qt/mode/visualization.hpp"
#include <app/core/collection.hpp>

namespace VTX::UI::Action::Visualization
{
	void ToggleCameraController ::execute()
	{
		if ( QT::MODE().getName() != QT::Mode::Visualization::ID )
			return;

		QT::Mode::Visualization & visualizationMode = dynamic_cast<QT::Mode::Visualization &>( QT::MODE() );

		const QT::Controller::BaseCameraController & currentCameraController
			= visualizationMode.getCurrentCameraController();

		const Util::Hashing::Hash & controllerHashKey = currentCameraController.getHashedCollectionID();

		const std::set<std::unique_ptr<QT::Controller::BaseController>> & controllerSet
			= visualizationMode.getCameraControllers();

		auto it = std::find_if(
			controllerSet.begin(),
			controllerSet.end(),
			[ controllerHashKey ]( const std::unique_ptr<QT::Controller::BaseController> & controllerPtr )
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
		if ( QT::MODE().getName() != QT::Mode::Visualization::ID )
			return;

		QT::Mode::Visualization & visualizationMode = dynamic_cast<QT::Mode::Visualization &>( QT::MODE() );
		visualizationMode.setCameraController( _controllerID );
	}

	void ApplyNextCameraController::execute()
	{
		if ( QT::MODE().getName() != QT::Mode::Visualization::ID )
			return;

		QT::Mode::Visualization & visualizationMode = dynamic_cast<QT::Mode::Visualization &>( QT::MODE() );
		visualizationMode.getCurrentCameraController();
	}
} // namespace VTX::UI::Action::Visualization
