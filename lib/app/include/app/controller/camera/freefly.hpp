#ifndef __VTX_UI_INTERNAL_CONTROLLER_CAMERA_FREEFLY__
#define __VTX_UI_INTERNAL_CONTROLLER_CAMERA_FREEFLY__

#include "app/core/controller/base_camera_controller.hpp"
#include "app/core/controller/controller_manager.hpp"
#include "app/core/input/key_mapping.hpp"
#include "app/settings.hpp"
#include <util/hashing.hpp>

namespace VTX::App::Controller::Camera
{

	class Freefly : public Core::Controller::BaseCameraController
	{
	  public:
		inline static const Util::CollectionKey COLLECTION_ID		 = "CONTROLLER_FREEFLY";
		inline static const VTX::Hash			HASHED_COLLECTION_ID = Util::hash( COLLECTION_ID );

	  private:
		inline static const Core::Controller::ControllerCollection::Registration<Freefly> _reg { COLLECTION_ID };

	  public:
		enum class Keys : int
		{
			MOVE_LEFT,
			MOVE_RIGHT,
			MOVE_FRONT,
			MOVE_BACK,
			MOVE_UP,
			MOVE_DOWN,
		};

	  public:
		Freefly()							= default;
		Freefly( const Freefly & p_source ) = default;
		~Freefly()							= default;

		void init() override;
		void setActive( const bool p_active ) override;

		inline VTX::Hash				getHashedCollectionID() const override { return HASHED_COLLECTION_ID; };
		std::unique_ptr<BaseController> clone() const { return std::make_unique<Freefly>( *this ); };

		float translationSpeed	 = Setting::Controller::TRANSLATION_SPEED_DEFAULT;
		float accelerationFactor = Setting::Controller::ACCELERATION_FACTOR_DEFAULT;
		float decelerationFactor = Setting::Controller::DECELERATION_FACTOR_DEFAULT;
		float rotationSpeed		 = Setting::Controller::ROTATION_SPEED_DEFAULT;
		bool  invertY			 = Setting::Controller::INVERT_Y_DEFAULT;
		bool  elasticityActive	 = Setting::Controller::ELASTICITY_ACTIVE_DEFAULT;
		float elasticityFactor	 = Setting::Controller::ELASTICITY_FACTOR_DEFAULT;

	  protected:
		void _updateInputs( const float & p_deltaTime ) override;

	  private:
		Core::Input::KeyMapping _mapping;
	};
} // namespace VTX::App::Controller::Camera
#endif
