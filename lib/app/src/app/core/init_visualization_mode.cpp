#include "app/core/init_visualization_mode.hpp"
#include "app/controller/camera/freefly.hpp"
#include "app/controller/camera/trackball.hpp"
#include "app/controller/picker/selection.hpp"
#include "app/controller/shortcut/debug.hpp"
#include "app/controller/shortcut/global.hpp"
#include "app/controller/shortcut/visualization.hpp"
#include "app/core/controller/controller_manager.hpp"

namespace VTX::App::Core
{
	void init( Mode::Visualization & p_visualization )
	{
		// TODO replace that with an initialisation with custom data.
		std::unique_ptr<Controller::BaseController> ptr
			= Util::Singleton<Controller::ControllerCollection>::get().instantiateItem(
				App::Controller::Camera::Freefly::HASHED_COLLECTION_ID
			);
		ptr->init();
		p_visualization.addCameraController( ptr );

		ptr = Util::Singleton<Controller::ControllerCollection>::get().instantiateItem(
			App::Controller::Camera::Trackball::HASHED_COLLECTION_ID
		);
		ptr->init();
		p_visualization.addCameraController( ptr );

		ptr = Util::Singleton<Controller::ControllerCollection>::get().instantiateItem(
			App::Controller::Picker::Selection::HASHED_COLLECTION_ID
		);
		ptr->init();
		p_visualization.addPickerController( ptr );

		ptr = Util::Singleton<Controller::ControllerCollection>::get().instantiateItem(
			App::Controller::Shortcut::Global::HASHED_COLLECTION_ID
		);
		ptr->init();
		p_visualization.addController( ptr );

		ptr = Util::Singleton<Controller::ControllerCollection>::get().instantiateItem(
			App::Controller::Shortcut::Visualization::HASHED_COLLECTION_ID
		);
		ptr->init();
		p_visualization.addController( ptr );

#ifndef VTX_PRODUCTION
		ptr = Util::Singleton<Controller::ControllerCollection>::get().instantiateItem(
			App::Controller::Shortcut::Debug::HASHED_COLLECTION_ID
		);
		ptr->init();
		p_visualization.addController( ptr );
#endif

		p_visualization.setCameraController( App::Controller::Camera::Trackball::HASHED_COLLECTION_ID );
		p_visualization.setPickerController( App::Controller::Picker::Selection::HASHED_COLLECTION_ID );
	}
} // namespace VTX::App::Core
