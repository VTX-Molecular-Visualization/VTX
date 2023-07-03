#ifndef __VTX_UI_QT_STATE_VISUALIZATION__
#define __VTX_UI_QT_STATE_VISUALIZATION__

#include "ui/qt/controller/base_camera_controller.hpp"
#include "ui/qt/state/base_state.hpp"

namespace VTX::UI::QT::State
{
	class Visualization : public BaseState
	{
	  public:
		Visualization();

		virtual void enter( void * const ) override;
		virtual void update( const float & p_deltaTime ) override;

		// inline const Controller::BaseCameraController * const getCurrentCameraController() const { return
		// getItem<Controller::BaseCameraController>( _cameraController ); }
		inline QT::Controller::BaseCameraController * const getCurrentCameraController()
		{
			return getController<QT::Controller::BaseCameraController>( _cameraController );
		}

		inline const App::Old::VTX_ID & getCurrentCameraControllerID() const { return _cameraController; }
		inline const App::Old::VTX_ID & getCurrentPickerID() const { return _pickerController; }

		void toggleCameraController();
		void setCameraController( const App::Old::VTX_ID & p_controllerId );
		void resetCameraController();
		void orientCameraController( const App::Old::Component::Object3D::Helper::AABB & );
		void orientCameraController( const Vec3f & p_position, const Quatf & p_orientation );

		void setPickerController( const App::Old::VTX_ID & p_pickerId );

		virtual void receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event ) override;

	  private:
	 App::Old::VTX_ID _cameraController = VTX::App::Old::Application::Setting::CONTROLLER_MODE_DEFAULT;
	 App::Old::VTX_ID _pickerController = VTX::App::Old::Application::Setting::PICKER_MODE_DEFAULT;
	};
} // namespace VTX::UI::QT::State
#endif
