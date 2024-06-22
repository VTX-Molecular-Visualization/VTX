#ifndef __VTX_UI_QT_MODE_VISUALIZATION__
#define __VTX_UI_QT_MODE_VISUALIZATION__

#include "app/core/controller/_fwd.hpp"
#include "app/core/controller/base_controller.hpp"
#include "app/core/controller/controller_manager.hpp"
#include "base_mode.hpp"
#include <set>
#include <util/callback.hpp>
#include <util/exceptions.hpp>

namespace VTX::App::Mode
{
	class Visualization : public BaseMode
	{
	  public:
		// inline static const ModeCollection::Registration<Visualization> _reg;

		inline static const std::string ID = "MODE_VISUALIZATION";

	  public:
		Visualization();
		Visualization( const Visualization & p_source ) = delete;
		const std::string & getName() const override { return ID; };

		// void init( VisualizationData & p_data );
		void enter() override;
		void update( float p_deltaTime ) override;
		void exit() override;

		void addCameraController( std::unique_ptr<Core::Controller::BaseController> & p_cameraControllerPtr );
		void addPickerController( std::unique_ptr<Core::Controller::BaseController> & p_pickerControllerPtr );
		void addController( std::unique_ptr<Core::Controller::BaseController> & p_controllerPtr );

		inline const Core::Controller::BaseCameraController & getCurrentCameraController() const
		{
			return *_currentCameraController;
		}
		inline Core::Controller::BaseCameraController & getCurrentCameraController()
		{
			return *_currentCameraController;
		}
		inline const Core::Controller::BasePickerController & getCurrentPickerController() const
		{
			return *_currentPickerController;
		}
		inline Core::Controller::BasePickerController & getCurrentPickerController()
		{
			return *_currentPickerController;
		}

		Core::Controller::BaseController & getController( const Util::CollectionKey & p_controllerKey );

		void setCameraController( const Util::Hashing::Hash & p_controllerKey );
		void setCameraController( const Util::CollectionKey & p_controllerKey );
		void setPickerController( const Util::Hashing::Hash & p_controllerHash );
		void setPickerController( const Util::CollectionKey & p_controllerKey );

		const std::set<std::unique_ptr<Core::Controller::BaseController>> & getCameraControllers() const
		{
			return _cameraControllers;
		}
		const std::set<std::unique_ptr<Core::Controller::BaseController>> & getPickerControllers() const
		{
			return _pickerControllers;
		}

		Util::Callback<Core::Controller::BaseCameraController> onCameraController;
		Util::Callback<Core::Controller::BasePickerController> onPickerController;

	  private:
		std::set<std::unique_ptr<Core::Controller::BaseController>> _cameraControllers
			= std::set<std::unique_ptr<Core::Controller::BaseController>>();
		Core::Controller::BaseCameraController * _currentCameraController = nullptr;

		std::set<std::unique_ptr<Core::Controller::BaseController>> _pickerControllers
			= std::set<std::unique_ptr<Core::Controller::BaseController>>();
		Core::Controller::BasePickerController * _currentPickerController = nullptr;

		std::set<std::unique_ptr<Core::Controller::BaseController>> _otherControllers
			= std::set<std::unique_ptr<Core::Controller::BaseController>>();

		void _affectCameraController( Core::Controller::BaseCameraController * p_ptr );
		void _affectPickerController( Core::Controller::BasePickerController * p_ptr );
	};
} // namespace VTX::App::Mode
#endif
