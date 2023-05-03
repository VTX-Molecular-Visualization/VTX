#ifndef __VTX_UI_QT_BASE_CAMERA_CONTROLLER__
#define __VTX_UI_QT_BASE_CAMERA_CONTROLLER__

#include "base_gamepad_controller.hpp"
#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"
#include "ui/core/controller/base_controller.hpp"
#include "ui/id.hpp"
#include <app/component/object3d/helper/aabb.hpp>
#include <app/component/render/camera.hpp>
#include <app/id.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT::Controller
{
	class BaseCameraController : public BaseMouseController, public BaseKeyboardController, public BaseGamepadController
	{
	  public:
		explicit BaseCameraController( App::Component::Render::Camera & p_camera ) : _camera( p_camera ) {}
		virtual ~BaseCameraController() = default;

		virtual const App::VTX_ID getID() const = 0;
		inline App::VTX_ID		  getTargetWidget() override { return UI::ID::Input::RENDER_WIDGET; }

		inline const bool	 isOrienting() const { return _isOrienting; }
		inline const Vec3f & getOrientStartingPosition() const { return _orientStartingPosition; }
		inline const Vec3f & getOrientTargetPosition() const { return _orientTargetPosition; }

		void update( const float & p_deltaTime ) override;

		virtual void reset() { _isOrienting = false; }

		virtual void orient( const App::Component::Object3D::Helper::AABB & p_aabb )
		{
			_orientTime = 0.f;
			_computeOrientPositions( p_aabb );
		}
		virtual void orient( const Vec3f & p_position, const Quatf & p_orientation )
		{
			_orientTime = 0.f;
			_computeOrientPositions( p_position, p_orientation );
		}

	  protected:
		const float ORIENT_DURATION	 = 0.5f;
		const float ORIENT_THRESHOLD = 1e-4f;

		App::Component::Render::Camera & _camera;

		bool  _isOrienting			  = false;
		float _orientTime			  = 0.f;
		Vec3f _orientStartingPosition = VEC3F_ZERO;
		Vec3f _orientTargetPosition	  = VEC3F_ZERO;
		Quatf _orientStartingRotation = QUATF_ID;
		Quatf _orientTargetRotation	  = QUATF_ID;

		virtual void _updateInputs( const float & )													  = 0;
		virtual void _computeOrientPositions( const App::Component::Object3D::Helper::AABB & p_aabb ) = 0;
		virtual void _computeOrientPositions( const Vec3f & p_position, const Quatf & p_orientation ) = 0;
		virtual void _updateOrient( const float & )													  = 0;
	};
} // namespace VTX::UI::QT::Controller
#endif
