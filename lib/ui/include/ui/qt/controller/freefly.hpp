#ifndef __VTX_UI_QT_CONTROLLER_FREEFLY__
#define __VTX_UI_QT_CONTROLLER_FREEFLY__

#include "ui/qt/controller/base_camera_controller.hpp"
#include "ui/qt/controller/controller_manager.hpp"
#include "ui/qt/input/input_manager.hpp"
#include <map>
#include <set>
#include <util/hashing.hpp>

namespace VTX::UI::QT::Controller
{
	class KeyMapping
	{
	  public:
		KeyMapping() {};
		KeyMapping( const std::map<int, std::set<Qt::Key>> & p_map ) : _map( p_map ) {};

		const std::set<Qt::Key> & operator[]( int p_keyEnum ) const { return _map.at( p_keyEnum ); }

	  private:
		std::map<int, std::set<Qt::Key>> _map = std::map<int, std::set<Qt::Key>>();
	};

	class Freefly : public BaseCameraController
	{
	  public:
		inline static const App::Core::CollectionKey COLLECTION_ID		  = "CONTROLLER_FREEFLY";
		inline static const Util::Hashing::Hash		 HASHED_COLLECTION_ID = Util::Hashing::hash( COLLECTION_ID );

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
		explicit Freefly();
		~Freefly() = default;

		inline Util::Hashing::Hash		getHashedCollectionID() const override { return HASHED_COLLECTION_ID; };
		std::unique_ptr<BaseController> clone() const override;

		float translationSpeed;
		float accelerationFactor;
		float decelerationFactor;
		float rotationSpeed;
		bool  invertY;
		bool  elasticityActive;
		float elasticityFactor;

	  protected:
		void _updateInputs( const float & p_deltaTime ) override;

	  private:
		const KeyMapping _mapping = KeyMapping( {
			{ int( Keys::MOVE_LEFT ), { Qt::Key::Key_Left, Input::InputManager::getKeyFromQwerty( Qt::Key::Key_A ) } },
			{ int( Keys::MOVE_RIGHT ), { Qt::Key::Key_Right } },
			{ int( Keys::MOVE_FRONT ), { Qt::Key::Key_Up, Input::InputManager::getKeyFromQwerty( Qt::Key::Key_W ) } },
			{ int( Keys::MOVE_BACK ), { Qt::Key::Key_Back } },
			{ int( Keys::MOVE_UP ), { Qt::Key::Key_R } },
			{ int( Keys::MOVE_DOWN ), { Qt::Key::Key_F } },
		} );
	};
} // namespace VTX::UI::QT::Controller
#endif
