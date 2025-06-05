#include "app/component/controller.hpp"

namespace VTX::App::Component
{
	App::Controller::Camera::GenericAnimation * const Controller::enableController(
		App::Controller::Camera::GenericAnimation p_animation
	)
	{
		Hash controller_hash = hash( p_animation );

		assert( not _controllers.has( controller_hash ) );
		assert( not _activeCallbacks.contains( controller_hash ) );

		_controllers.set( controller_hash, new App::Controller::Camera::GenericAnimation( std::move( p_animation ) ) );
		App::Controller::Camera::GenericAnimation * const controller
			= _controllers.get<App::Controller::Camera::GenericAnimation>( controller_hash );

		controller->setCamera( &SCENE().getCamera() );

		// Register update callback.
		Util::CallbackId id = addUpdateFunction( [ controller ]( const float p_delta, const float p_elapsed )
												 { controller->update( p_delta, p_elapsed ); } );

		// Save callback id.
		_activeCallbacks.emplace( controller_hash, id );

		// Trigger callbacks.
		onControllerEnabled( controller_hash );

		return controller;
	}

	void Controller::disableController( Hash p_hash )
	{
		// Not created.
		if ( not _controllers.has( p_hash ) )
		{
			return;
		}

		assert( _activeCallbacks.contains( p_hash ) );

		_controllers.remove( p_hash );

		// Unregister update callback.
		removeUpdateFunction( _activeCallbacks.at( p_hash ) );
		_activeCallbacks.erase( p_hash );
	}

	void Controller::launchAnimation( App::Controller::Camera::GenericAnimation p_animation )
	{
		// Already running.
		Hash controller_hash = hash( p_animation );
		if ( _controllers.has( controller_hash ) )
		{
			return;
		}

		auto * controller = enableController( std::move( p_animation ) );

		auto & transformComponent
			= App::ECS_REGISTRY().getComponent<App::Component::Scene::Transform>( SCENE().getCamera() );

		// Connect animation callbacks.
		controller->onAnimationProgress() +=
			[ &transformComponent ]( const Vec3f & p_position, const Quatf & p_rotation )
		{
			transformComponent.setPosition( p_position );
			transformComponent.setRotation( p_rotation );
		};

		controller->onAnimationEnd() += [ this, controller_hash ]( const Vec3f & p_target )
		{
			SCENE().getCamera().setTargetWorld( p_target );
			APP::onEndOfFrameOneShot += [ this, controller_hash ]() { disableController( controller_hash ); };
		};

		controller->play();
	}

} // namespace VTX::App::Component
