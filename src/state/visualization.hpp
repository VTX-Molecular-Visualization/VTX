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
			virtual void exit() override;

			virtual void update( const float & p_deltaTime ) override;

			// inline const Controller::BaseCameraController * const getCurrentCameraController() const { return getItem<Controller::BaseCameraController>( _cameraController ); }
			inline Controller::BaseCameraController * const getCurrentCameraController() { return getItem<Controller::BaseCameraController>( _cameraController ); }
			inline const ID::VTX_ID &						getCurrentCameraControllerID() const { return _cameraController; }

			void toggleCameraController();
			void setCameraController( const ID::VTX_ID & p_controllerId );
			void resetCameraController();
			void orientCameraController( const Math::AABB & );

			virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

		  private:
			ID::VTX_ID _cameraController = ID::Controller::TRACKBALL;
		};
	} // namespace State
} // namespace VTX
#endif
