#ifndef __VTX_APP_COMPONENT_CONTROLLER__
#define __VTX_APP_COMPONENT_CONTROLLER__

#include "app/application/scene.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/component/scene/updatable.hpp"
#include "app/controller/camera/animation.hpp"
#include "app/core/animation/concepts.hpp"
#include "app/core/controller/base_controller.hpp"
#include "app/core/controller/concepts.hpp"
#include "app/core/ecs/ecs_system.hpp"
#include <util/callback.hpp>
#include <util/collection.hpp>
#include <util/hashing.hpp>
#include <util/types.hpp>

namespace VTX::App::Component
{
	class Controller : public Scene::Updatable
	{
	  public:
		Controller()					 = default;
		Controller( const Controller & ) = delete;

		App::Controller::Camera::GenericAnimation * const enableController(
			App::Controller::Camera::GenericAnimation p_animation
		);
		template<Core::Controller::ConceptController C, typename... Args>
		C * const enableController( Args &&... p_args )
		{
			Hash hash = Util::hash<C>();

			assert( not _controllers.has<C>() );
			assert( not _activeCallbacks.contains( hash ) );

			C * const controller = _controllers.create<C>( std::forward<Args>( p_args )... );
			controller->setCamera( &SCENE().getCamera() );

			// Register update callback.
			Util::CallbackId id = addUpdateFunction( [ controller ]( const float p_delta, const float p_elapsed )
													 { controller->update( p_delta, p_elapsed ); } );

			// Save callback id.
			_activeCallbacks.emplace( hash, id );

			// Trigger callbacks.
			onControllerEnabled( Util::hash<C>() );

			return controller;
		}

		void disableController( Hash );
		template<Core::Controller::ConceptController C>
		void disableController()
		{
			const Hash hash = Util::hash<C>();

			// Not created.
			if ( not _controllers.has<C>() )
			{
				return;
			}

			assert( _activeCallbacks.contains( hash ) );

			_controllers.remove<C>();

			// Unregister update callback.
			removeUpdateFunction( _activeCallbacks.at( hash ) );
			_activeCallbacks.erase( hash );
		}

		void launchAnimation( App::Controller::Camera::GenericAnimation p_animation );

		inline bool isControllerEnabled( Hash p_hash ) const { return _activeCallbacks.contains( p_hash ); }
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
