#ifndef __VTX_APP_COMPONENT_CONTROLLER__
#define __VTX_APP_COMPONENT_CONTROLLER__

#include "app/application/scene.hpp"
#include "app/core/controller/base_controller_camera.hpp"
#include "app/core/controller/concepts.hpp"
#include "app/core/ecs/base_component.hpp"
#include "app/vtx_app.hpp"
#include <util/callback.hpp>
#include <util/collection.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>

namespace VTX::App::Component
{
	class Controller : public Core::ECS::BaseComponent
	{
		// TODO: use an unique camera controller?
		// + aniamtion controller.
	  public:
		Controller()					 = default;
		Controller( const Controller & ) = delete;

		template<Core::Controller::ConceptController C>
		void enableController()
		{
			Hash hash = Util::hash<C>();

			assert( not _controllers.has<C>() );
			assert( not _activeCallbacks.contains( hash ) );

			C * const controller = _controllers.create<C>();

			if constexpr ( std::derived_from<C, Core::Controller::BaseControllerCamera> )
			{
				static_cast<Core::Controller::BaseControllerCamera *>( controller )->setCamera( &SCENE().getCamera() );
			}

			// Register update callback.
			Util::CallbackId id = APP::onUpdate += [ controller ]( const float p_delta, const float p_elapsed )
			{ controller->update( p_delta, p_elapsed ); };

			// Save callback id.
			_activeCallbacks.emplace( hash, id );

			// Trigger callbacks.
			onControllerEnabled( Util::hash<C>() );
		}

		template<Core::Controller::ConceptController C>
		void disableController()
		{
			const Hash hash = Util::hash<C>();

			// Not created.
			assert( _controllers.has<C>() );
			assert( _activeCallbacks.contains( hash ) );

			_controllers.remove<C>();

			// Unregister update callback.
			APP::onUpdate -= _activeCallbacks.at( hash );
			_activeCallbacks.erase( hash );
		}

		template<Core::Controller::ConceptController C>
		bool isControllerEnabled() const
		{
			return _activeCallbacks.contains( Util::hash<C>() );
		}

		Util::Callback<Hash> onControllerEnabled;

	  private:
		Util::Collection<std::unique_ptr<Core::Controller::BaseController>> _controllers;
		//    TODO: improve collection to handle basic types.
		std::unordered_map<Hash, Util::CallbackId> _activeCallbacks;
	};
} // namespace VTX::App::Component

#endif
