#ifndef __VTX_BASE_CAMERA_CONTROLLER__
#define __VTX_BASE_CAMERA_CONTROLLER__

#include "base_controller.hpp"
#include "base_gamepad_controller.hpp"
#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"
#include "ui/id.hpp"
#include <app/old/component/object3d/helper/aabb.hpp>
#include <app/old/component/render/camera.hpp>
#include <app/old/id.hpp>
#include <app/old/internal/scene/camera_manager.hpp>

namespace VTX
{
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
			explicit BaseCameraController( App::Old::Internal::Scene::CameraManager & p_cameraManager ) :
				_cameraManager( p_cameraManager )
			{
			}
			virtual ~BaseCameraController() = default;

			virtual const App::Old::VTX_ID getID() const = 0;
			inline App::Old::VTX_ID		  getTargetWidget() override { return UI::ID::Input::RENDER_WIDGET; }

			inline const bool	 isOrienting() const { return _isOrienting; }
			inline const Vec3f & getOrientStartingPosition() const { return _orientStartingPosition; }
			inline const Vec3f & getOrientTargetPosition() const { return _orientTargetPosition; }

			void update( const float & p_deltaTime ) override;

			virtual void reset() { _isOrienting = false; }

			virtual void orient( const App::Old::Component::Object3D::Helper::AABB & p_aabb );
			virtual void orient( const Vec3f & p_position, const Quatf & p_orientation );

		  protected:
			App::Old::Internal::Scene::CameraManager & _cameraManager;

			bool  _isOrienting			  = false;
			float _orientTime			  = 0.f;
			Vec3f _orientStartingPosition = VEC3F_ZERO;
			Vec3f _orientTargetPosition	  = VEC3F_ZERO;
			Quatf _orientStartingRotation = QUATF_ID;
			Quatf _orientTargetRotation	  = QUATF_ID;

			inline App::Old::Component::Render::Camera & _camera() const { return *_cameraManager.getCamera(); }

			virtual void _updateInputs( const float & )													  = 0;
			virtual void _computeOrientPositions( const App::Old::Component::Object3D::Helper::AABB & p_aabb ) = 0;
			virtual void _computeOrientPositions( const Vec3f & p_position, const Quatf & p_orientation ) = 0;
			virtual void _updateOrient( const float & )													  = 0;
		};
	} // namespace Controller
} // namespace VTX
#endif
