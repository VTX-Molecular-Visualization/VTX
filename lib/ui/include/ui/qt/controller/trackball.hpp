#ifndef __VTX_UI_QT_CONTROLLER_TRACKBALL__
#define __VTX_UI_QT_CONTROLLER_TRACKBALL__

#include "ui/core/input/key_mapping.hpp"
#include "ui/internal/all_settings.hpp"
#include "ui/qt/controller/base_camera_controller.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include <app/component/render/camera.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT::Controller
{
	class Trackball : public BaseCameraController
	{
	  public:
		inline static const App::Core::CollectionKey COLLECTION_ID		  = "CONTROLLER_TRACKBALL";
		inline static const VTX::Util::Hashing::Hash HASHED_COLLECTION_ID = VTX::Util::Hashing::hash( COLLECTION_ID );

	  private:
		inline static const ControllerCollection::Registration<Trackball> _reg { COLLECTION_ID };
		inline static const float										  _CONTROLLER_ELASTICITY_THRESHOLD = 1e-4f;

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

		void setActive( const bool p_active );
		void reset() override;

		inline VTX::Util::Hashing::Hash getHashedCollectionID() const override { return HASHED_COLLECTION_ID; };
		std::unique_ptr<BaseController> clone() const { return std::make_unique<Trackball>( *this ); };

		inline const Vec3f & getTarget() const { return _target; }
		inline void			 setTarget( const Vec3f & p_target ) { _target = p_target; }
		inline void			 setDistanceForced( const float & p_distance ) { _distanceForced = p_distance; }

		Vec3f targetSimulationFromCamera( const App::Component::Render::Camera & p_camera ) const;

		float translationSpeed	 = Internal::Controller::TRANSLATION_SPEED_DEFAULT;
		float accelerationFactor = Internal::Controller::ACCELERATION_FACTOR_DEFAULT;
		float decelerationFactor = Internal::Controller::DECELERATION_FACTOR_DEFAULT;
		float rotationSpeed		 = Internal::Controller::ROTATION_SPEED_DEFAULT;
		bool  invertY			 = Internal::Controller::INVERT_Y_DEFAULT;
		bool  elasticityActive	 = Internal::Controller::ELASTICITY_ACTIVE_DEFAULT;
		float elasticityFactor	 = Internal::Controller::ELASTICITY_FACTOR_DEFAULT;

	  protected:
		void _updateInputs( const float & ) override;
		void _updateElasticity( const float & );

	  private:
		Core::Input::KeyMapping _mapping;

		Vec3f _target		  = VEC3F_ZERO;
		float _distanceForced = 0.f;
		Vec3f _velocity		  = VEC3F_ZERO;

		bool _needUpdate = false;
	};
} // namespace VTX::UI::QT::Controller
#endif
