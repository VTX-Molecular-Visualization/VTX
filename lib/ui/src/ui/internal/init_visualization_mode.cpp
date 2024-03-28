#include "ui/internal/init_visualization_mode.hpp"
#include "ui/internal/controller/camera/freefly.hpp"
#include "ui/internal/controller/camera/trackball.hpp"
#include "ui/internal/controller/picker/selection.hpp"
#include "ui/internal/controller/shortcut/debug.hpp"
#include "ui/internal/controller/shortcut/global.hpp"
#include "ui/internal/controller/shortcut/visualization.hpp"
#include "ui/qt/controller/controller_manager.hpp"

namespace VTX::UI::Internal
{
	void init( QT::Mode::Visualization & p_visualization )
	{
		// TODO replace that with an initialisation with custom data.
		std::unique_ptr<QT::Controller::BaseController> ptr
			= QT::Controller::ControllerCollection::get().instantiateItem(
				Controller::Camera::Freefly::HASHED_COLLECTION_ID
			);
		ptr->init();
		p_visualization.addCameraController( ptr );

		ptr = QT::Controller::ControllerCollection::get().instantiateItem(
			Controller::Camera::Trackball::HASHED_COLLECTION_ID
		);
		ptr->init();
		p_visualization.addCameraController( ptr );

		ptr = QT::Controller::ControllerCollection::get().instantiateItem(
			Controller::Picker::Selection::HASHED_COLLECTION_ID
		);
		ptr->init();
		p_visualization.addPickerController( ptr );

		ptr = QT::Controller::ControllerCollection::get().instantiateItem(
			Controller::Shortcut::Global::HASHED_COLLECTION_ID
		);
		ptr->init();
		p_visualization.addController( ptr );

		ptr = QT::Controller::ControllerCollection::get().instantiateItem(
			Controller::Shortcut::Visualization::HASHED_COLLECTION_ID
		);
		ptr->init();
		p_visualization.addController( ptr );

#ifndef VTX_PRODUCTION
		ptr = QT::Controller::ControllerCollection::get().instantiateItem(
			Controller::Shortcut::Debug::HASHED_COLLECTION_ID
		);
		ptr->init();
		p_visualization.addController( ptr );
#endif

		p_visualization.setCameraController( Controller::Camera::Trackball::HASHED_COLLECTION_ID );
		p_visualization.setPickerController( Controller::Picker::Selection::HASHED_COLLECTION_ID );
	}
} // namespace VTX::UI::Internal
