#ifndef __VTX_STATE_VISUALIZATION__
#define __VTX_STATE_VISUALIZATION__

#include "base_state.hpp"
#include "controller/base_camera_controller.hpp"

namespace VTX
{
	namespace State
	{
		class Visualization : public BaseState
		{
		  public:
			Visualization();

			virtual void enter( void * const ) override;
			virtual void update( const float & p_deltaTime ) override;

			inline const Controller::BaseCameraController * const getCurrentCameraController() const
			{
				return getController<Controller::BaseCameraController>( _cameraController );
			}
			inline Controller::BaseCameraController * const getCurrentCameraController()
			{
				return getController<Controller::BaseCameraController>( _cameraController );
			}

			inline const ID::VTX_ID & getCurrentCameraControllerID() const { return _cameraController; }
			inline const ID::VTX_ID & getCurrentPickerID() const { return _pickerController; }

			void toggleCameraController();
			void setCameraController( const ID::VTX_ID & p_controllerId );
			void resetCameraController();
			void orientCameraController( const Object3D::Helper::AABB & );
			void orientCameraController( const Vec3f & p_position, const Quatf & p_orientation );

			void setPickerController( const ID::VTX_ID & p_pickerId );

			virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

		  private:
			ID::VTX_ID _cameraController = Setting::CONTROLLER_MODE_DEFAULT;
			ID::VTX_ID _pickerController = Setting::PICKER_MODE_DEFAULT;
		};
	} // namespace State
} // namespace VTX
#endif
