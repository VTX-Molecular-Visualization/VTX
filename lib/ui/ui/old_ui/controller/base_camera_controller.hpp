#ifndef __VTX_BASE_CAMERA_CONTROLLER__
#define __VTX_BASE_CAMERA_CONTROLLER__

#include "base_controller.hpp"
#include "base_gamepad_controller.hpp"
#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"
#include <app/old_app/id.hpp>
#include <app/old_app/object3d/camera.hpp>
#include <app/old_app/object3d/camera_manager.hpp>
#include <app/old_app/object3d/helper/aabb.hpp>

namespace VTX
{
	namespace Object3D
	{
		class CameraManager;
		class Camera;
	} // namespace Object3D

	namespace Controller
	{
		class BaseCameraController :
			public BaseMouseController,
			public BaseKeyboardController,
			public BaseGamepadController
		{
		  protected:
			inline static const float ORIENT_DURATION  = 0.5f;
			inline static const float ORIENT_THRESHOLD = 1e-4f;

		  public:
			explicit BaseCameraController( Object3D::CameraManager & p_cameraManager ) :
				_cameraManager( p_cameraManager )
			{
			}
			virtual ~BaseCameraController() = default;

			virtual const ID::VTX_ID getID() const = 0;
			inline ID::VTX_ID		 getTargetWidget() override { return ID::UI::Input::RENDER_WIDGET; }

			inline const bool	 isOrienting() const { return _isOrienting; }
			inline const Vec3f & getOrientStartingPosition() const { return _orientStartingPosition; }
			inline const Vec3f & getOrientTargetPosition() const { return _orientTargetPosition; }

			void update( const float & p_deltaTime ) override;

			virtual void reset() { _isOrienting = false; }

			virtual void orient( const Object3D::Helper::AABB & p_aabb );
			virtual void orient( const Vec3f & p_position, const Quatf & p_orientation );

		  protected:
			Object3D::CameraManager & _cameraManager;

			bool  _isOrienting			  = false;
			float _orientTime			  = 0.f;
			Vec3f _orientStartingPosition = VEC3F_ZERO;
			Vec3f _orientTargetPosition	  = VEC3F_ZERO;
			Quatf _orientStartingRotation = QUATF_ID;
			Quatf _orientTargetRotation	  = QUATF_ID;

			inline Object3D::Camera & _camera() const { return *_cameraManager.getCamera(); }

			virtual void _updateInputs( const float & )													  = 0;
			virtual void _computeOrientPositions( const Object3D::Helper::AABB & p_aabb )				  = 0;
			virtual void _computeOrientPositions( const Vec3f & p_position, const Quatf & p_orientation ) = 0;
			virtual void _updateOrient( const float & )													  = 0;
		};
	} // namespace Controller
} // namespace VTX
#endif
