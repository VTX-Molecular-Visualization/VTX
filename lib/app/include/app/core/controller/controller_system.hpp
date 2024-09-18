#ifndef __VTX_APP_CORE_CONTROLLER_CONTROLLER_SYSTEM__
#define __VTX_APP_CORE_CONTROLLER_CONTROLLER_SYSTEM__

#include "app/application/scene.hpp"
#include "concepts.hpp"
#include <util/callback.hpp>
#include <util/collection.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Core::Controller
{

	class ControllerSystem
	{
	  public:
		template<Controller::ConceptController C>
		void enableController()
		{
			C * const controller = _controllers.getOrCreate<C>();
			assert( not _activeCallbacks.contains( controller->getName() ) );

			if constexpr ( std::derived_from<C, Controller::BaseControllerCamera> )
			{
				static_cast<Controller::BaseControllerCamera *>( controller )->setCamera( &SCENE().getCamera() );
			}

			// Register update callback.
			Util::CallbackId id = APP::onUpdate += [ controller ]( const float p_delta, const float p_elapsed )
			{ controller->update( p_delta, p_elapsed ); };

			// Save callback id.
			_activeCallbacks[ controller->getName() ] = id;

			// Set controller active.
			controller->setActive( true );

			// Trigger callbacks.
			onControllerEnabled( controller->getName() );
		}

		template<Controller::ConceptController C>
		void disableController()
		{
			// Not created.
			if ( not _controllers.has<C>() )
			{
				return;
			}

			C * const controller = _controllers.get<C>();
			// Not active.
			if ( not _activeCallbacks.contains( controller->getName() ) )
			{
				return;
			}

			// Unregister update callback.
			APP::onUpdate -= _activeCallbacks[ controller->getName() ];
			_activeCallbacks.erase( controller->getName() );
			controller->setActive( false );
		}

		template<Controller::ConceptController C>
		bool isControllerEnabled() const
		{
			return _activeCallbacks.contains( C::NAME );
		}

		Util::Callback<Name> onControllerEnabled;

	  private:
		Util::Collection<std::unique_ptr<Controller::BaseController>> _controllers;
		std::unordered_map<Name, Util::CallbackId>					  _activeCallbacks;
	};

} // namespace VTX::App::Core::Controller

namespace VTX::App
{
	inline Core::Controller::ControllerSystem & CONTROLLER_SYSTEM()
	{
		return Util::Singleton<Core::Controller::ControllerSystem>::get();
	}
} // namespace VTX::App

#endif
