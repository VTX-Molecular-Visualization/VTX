#ifndef __VTX_UI_QT_CONTROLLER_FREEFLY__
#define __VTX_UI_QT_CONTROLLER_FREEFLY__

#include "ui/core/input/key_mapping.hpp"
#include "ui/internal/all_settings.hpp"
#include "ui/qt/controller/base_camera_controller.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include <util/hashing.hpp>

namespace VTX::UI::QT::Controller
{
	class Freefly : public BaseCameraController
	{
	  public:
		inline static const App::Core::CollectionKey COLLECTION_ID		  = "CONTROLLER_FREEFLY";
		inline static const VTX::Util::Hashing::Hash HASHED_COLLECTION_ID = VTX::Util::Hashing::hash( COLLECTION_ID );

	  private:
		inline static const ControllerCollection::Registration<Freefly> _reg { COLLECTION_ID };

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

		inline VTX::Util::Hashing::Hash getHashedCollectionID() const override { return HASHED_COLLECTION_ID; };
		std::unique_ptr<BaseController> clone() const { return std::make_unique<Freefly>( *this ); };

		float translationSpeed	 = Internal::Controller::TRANSLATION_SPEED_DEFAULT;
		float accelerationFactor = Internal::Controller::ACCELERATION_FACTOR_DEFAULT;
		float decelerationFactor = Internal::Controller::DECELERATION_FACTOR_DEFAULT;
		float rotationSpeed		 = Internal::Controller::ROTATION_SPEED_DEFAULT;
		bool  invertY			 = Internal::Controller::INVERT_Y_DEFAULT;
		bool  elasticityActive	 = Internal::Controller::ELASTICITY_ACTIVE_DEFAULT;
		float elasticityFactor	 = Internal::Controller::ELASTICITY_FACTOR_DEFAULT;

	  protected:
		void _updateInputs( const float & p_deltaTime ) override;

	  private:
		Core::Input::KeyMapping _mapping;
	};
} // namespace VTX::UI::QT::Controller
#endif
