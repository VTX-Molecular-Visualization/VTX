#include "ui/action/visualization.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include "ui/qt/mode/base_mode.hpp"
#include "ui/qt/mode/visualization.hpp"

namespace VTX::UI::Action::Visualization
{
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
