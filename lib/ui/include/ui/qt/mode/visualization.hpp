#ifndef __VTX_UI_QT_MODE_VISUALIZATION__
#define __VTX_UI_QT_MODE_VISUALIZATION__

#include "ui/qt/controller/_fwd.hpp"
#include "ui/qt/controller/base_controller.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include "ui/qt/mode/base_mode.hpp"
#include "ui/qt/mode/mode_collection.hpp"
#include <app/core/callback_event.hpp>
#include <set>
#include <util/exceptions.hpp>

namespace VTX::UI::QT::Mode
{
	// class VisualizationData
	//{
	//   public:
	//	std::set<std::unique_ptr<Controller::BaseCameraController>> cameraControllers
	//		= std::set<std::unique_ptr<Controller::BaseCameraController>>();
	//	std::set<std::unique_ptr<Controller::BasePickerController>> pickerControllers
	//		= std::set<std::unique_ptr<Controller::BasePickerController>>();
	//	std::set<std::unique_ptr<Controller::BaseController>> otherControllers
	//		= std::set<std::unique_ptr<Controller::BaseController>>();
	// };

	class Visualization : public BaseMode
	{
	  private:
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

		void addCameraController( std::unique_ptr<Controller::BaseController> & p_cameraControllerPtr );
		void addPickerController( std::unique_ptr<Controller::BaseController> & p_pickerControllerPtr );
		void addController( std::unique_ptr<Controller::BaseController> & p_controllerPtr );

		inline const Controller::BaseCameraController & getCurrentCameraController() const
		{
			return *_currentCameraController;
		}
		inline Controller::BaseCameraController & getCurrentCameraController() { return *_currentCameraController; }
		inline const Controller::BasePickerController & getCurrentPickerController() const
		{
			return *_currentPickerController;
		}
		inline Controller::BasePickerController & getCurrentPickerController() { return *_currentPickerController; }

		Controller::BaseController & getController( const App::Core::CollectionKey & p_controllerKey );

		void setCameraController( const App::Core::CollectionKey & p_controllerKey );
		void setPickerController( const App::Core::CollectionKey & p_controllerKey );
		void resetCameraController();
		// void orientCameraController( const App::Old::Component::Object3D::Helper::AABB & );
		// void orientCameraController( const Vec3f & p_position, const Quatf & p_orientation );

		// virtual void receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event ) override;

		App::Core::CallbackEmitter<Controller::BaseCameraController &> onCameraControllerChange
			= App::Core::CallbackEmitter<Controller::BaseCameraController &>();
		App::Core::CallbackEmitter<Controller::BasePickerController &> onPickerControllerChange
			= App::Core::CallbackEmitter<Controller::BasePickerController &>();

	  private:
		std::set<std::unique_ptr<Controller::BaseController>> _cameraControllers
			= std::set<std::unique_ptr<Controller::BaseController>>();
		Controller::BaseCameraController * _currentCameraController = nullptr;

		std::set<std::unique_ptr<Controller::BaseController>> _pickerControllers
			= std::set<std::unique_ptr<Controller::BaseController>>();
		Controller::BasePickerController * _currentPickerController = nullptr;

		std::set<std::unique_ptr<Controller::BaseController>> _otherControllers
			= std::set<std::unique_ptr<Controller::BaseController>>();
	};
} // namespace VTX::UI::QT::Mode
#endif
