#ifndef __VTX_UI_QT_CONTROLLER_CAMERA_ANIMATION_CONTROLLER__
#define __VTX_UI_QT_CONTROLLER_CAMERA_ANIMATION_CONTROLLER__

#include "ui/core/input/keys.hpp"
#include "ui/qt/controller/base_controller.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include <util/hashing.hpp>

namespace VTX::UI::QT::Controller
{
	class CameraAnimationController : public BaseController
	{
	  public:
		inline static const App::Core::CollectionKey COLLECTION_ID		  = "CONTROLLER_CAMERA_ANIMATION";
		inline static const VTX::Util::Hashing::Hash HASHED_COLLECTION_ID = VTX::Util::Hashing::hash( COLLECTION_ID );

	  private:
		inline static const ControllerCollection::Registration<CameraAnimationController> _reg { COLLECTION_ID };

	  public:
		CameraAnimationController()												= default;
		CameraAnimationController( const CameraAnimationController & p_source ) = default;
		~CameraAnimationController()											= default;

		void update( const float & p_deltaTime ) override;

		inline VTX::Util::Hashing::Hash getHashedCollectionID() const override { return HASHED_COLLECTION_ID; };
		std::unique_ptr<BaseController> clone() const override
		{
			return std::make_unique<CameraAnimationController>( *this );
		};
	};
} // namespace VTX::UI::QT::Controller
#endif
