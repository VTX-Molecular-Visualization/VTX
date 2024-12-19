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

		template<Core::Controller::ConceptController C>
		void disableController()
		{
			const Hash hash = Util::hash<C>();

			// Not created.
			assert( _controllers.has<C>() );
			assert( _activeCallbacks.contains( hash ) );

			_controllers.remove<C>();

			// Unregister update callback.
			removeUpdateFunction( _activeCallbacks.at( hash ) );
			_activeCallbacks.erase( hash );
		}

		template<Core::Animation::ConceptAnimation A, typename... Args>
		void launchAnimation( Args &&... p_args )
		{
			// Already running.
			if ( _controllers.has<App::Controller::Camera::Animation<A>>() )
			{
				return;
			}

			auto * controller
				= enableController<App::Controller::Camera::Animation<A>>( std::forward<Args>( p_args )... );

			auto & transformComponent
				= App::ECS_REGISTRY().getComponent<App::Component::Scene::Transform>( SCENE().getCamera() );

			// Connect animation callbacks.
			controller->onAnimationProgress() +=
				[ &transformComponent ]( const Vec3f & p_position, const Quatf & p_rotation )
			{
				transformComponent.setPosition( p_position );
				transformComponent.setRotation( p_rotation );
			};

			controller->onAnimationEnd() += [ this ]( const Vec3f & p_target )
			{
				SCENE().getCamera().setTargetWorld( p_target );
				APP::onEndOfFrameOneShot += [ this ]() { disableController<App::Controller::Camera::Animation<A>>(); };
			};

			controller->play();
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
