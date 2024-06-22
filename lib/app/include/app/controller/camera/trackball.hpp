#ifndef __VTX_UI_INTERNAL_CONTROLLER_CAMERA_TRACKBALL__
#define __VTX_UI_INTERNAL_CONTROLLER_CAMERA_TRACKBALL__

#include "app/core/controller/base_camera_controller.hpp"
#include "app/core/controller/controller_manager.hpp"
#include "app/core/input/key_mapping.hpp"
#include "app/settings.hpp"
#include <app/component/render/camera.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>

namespace VTX::App::Controller::Camera
{

	class Trackball : public Core::Controller::BaseCameraController
	{
	  public:
		inline static const Util::CollectionKey COLLECTION_ID		 = "CONTROLLER_TRACKBALL";
		inline static const Util::Hashing::Hash HASHED_COLLECTION_ID = Util::Hashing::hash( COLLECTION_ID );

	  private:
		inline static const Core::Controller::ControllerCollection::Registration<Trackball> _reg { COLLECTION_ID };
		inline static const float _CONTROLLER_ELASTICITY_THRESHOLD = 1e-4f;

	  public:
		enum class Keys : int
		{
			MOVE_FRONT,
			MOVE_BACK,
			ROTATE_LEFT,
			ROTATE_RIGHT,
			ROTATE_UP,
			ROTATE_DOWN,
			ROLL_LEFT,
			ROLL_RIGHT,
		};

	  public:
		Trackball()								= default;
		Trackball( const Trackball & p_source ) = default;
		~Trackball()							= default;

		void init() override;
		void setActive( const bool p_active ) override;

		void reset() override;

		inline VTX::Util::Hashing::Hash getHashedCollectionID() const override { return HASHED_COLLECTION_ID; };
		std::unique_ptr<BaseController> clone() const { return std::make_unique<Trackball>( *this ); };

		Vec3f targetSimulationFromCamera( const App::Component::Render::Camera & p_camera ) const;

		float translationSpeed	 = Setting::Controller::TRANSLATION_SPEED_DEFAULT;
		float accelerationFactor = Setting::Controller::ACCELERATION_FACTOR_DEFAULT;
		float decelerationFactor = Setting::Controller::DECELERATION_FACTOR_DEFAULT;
		float rotationSpeed		 = Setting::Controller::ROTATION_SPEED_DEFAULT;
		bool  invertY			 = Setting::Controller::INVERT_Y_DEFAULT;
		bool  elasticityActive	 = Setting::Controller::ELASTICITY_ACTIVE_DEFAULT;
		float elasticityFactor	 = Setting::Controller::ELASTICITY_FACTOR_DEFAULT;

	  protected:
		void _updateInputs( const float & ) override;
		void _updateElasticity( const float & );

	  private:
		Core::Input::KeyMapping _mapping;

		Vec3f _velocity = VEC3F_ZERO;

		bool _needUpdate = false;
	};
} // namespace VTX::App::Controller::Camera
#endif
