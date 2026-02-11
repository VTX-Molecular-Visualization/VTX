#ifndef __VTX_APP_PASS_CAMERA_UPDATER__
#define __VTX_APP_PASS_CAMERA_UPDATER__

#include "app/controller/concepts.hpp"
#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"
#include <util/constants.hpp>

namespace VTX::App::Pass
{

	/**
	 * @brief System that updates camera view and projection matrices when camera or transform components are updated.
	 */
	class CameraUpdater : public IPass
	{
	  public:
		using UpdateDelegate = Util::EventHub::Delegate<void( const float, Util::Math::Transform & )>;

		/**
		 * @brief Constructor.
		 */
		CameraUpdater( const ECS::Entity & );

		/**
		 * @brief Update each frame.
		 */
		inline void update( const float p_delta, const float )
		{
			if ( _controller )
			{
				_controllerUpdateDelegate( p_delta, _transform );
			}
		}

		/**
		 * @brief Create and set a controller.
		 */
		template<Controller::ConceptController C, typename... Args>
		void setController( Args &&... p_args )
		{
			auto controller = std::make_unique<C>( std::forward<Args>( p_args )... );
			_controllerUpdateDelegate.template connect<&C::update>( controller.get() );
			_controller = std::move( controller );
		}

	  private:
		/**
		 * @brief Camera entity.
		 */
		const ECS::Entity _entity;

		/**
		 * @brief Camera transform reference (avoid get component in update).
		 */
		Util::Math::Transform & _transform;

		/**
		 * @brief Current controller.
		 */
		std::unique_ptr<Controller::IController> _controller;

		/**
		 * @brief Update delegate.
		 */
		UpdateDelegate _controllerUpdateDelegate;

		/**
		 * @brief On update.
		 */
		void _onUpdate();

	}; // namespace VTX::App::Pass
} // namespace VTX::App::Pass

#endif
