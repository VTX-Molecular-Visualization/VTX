#ifndef __VTX_APP_PASS_CAMERA_UPDATER__
#define __VTX_APP_PASS_CAMERA_UPDATER__

#include "app/controller/concepts.hpp"
#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/input/input_manager.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/services.hpp"
#include "app/setting/controller.hpp"
#include <renderer/camera.hpp>
#include <util/constants.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Pass
{

	/**
	 * @brief System that updates camera view and projection matrices when camera or transform components are updated.
	 */
	class CameraUpdater : public IPass
	{
	  public:
		/**
		 * @brief Controller update fonction (corresponds to ConceptController).
		 */
		using UpdateDelegate = Util::EventHub::Delegate<
			bool( const float, const Setting::Controller &, Util::Math::Transform &, Vec3f & )>;

		/**
		 * @brief Constructor.
		 */
		CameraUpdater( const Entity & );

		/**
		 * @brief Update each frame.
		 */
		inline void update( const float p_delta, const float )
		{
			if ( not _controllers.empty() )
			{
				const auto & settings = REG().get<Setting::Controller>( _entity );
				auto &		 transform = REG().get<Util::Math::Transform>( _entity );
				auto &		 target	   = REG().get<Renderer::Camera>( _entity ).target;

				// If ended, go to next controller.
				if ( not _controllers.front().update( p_delta, settings, transform, target ) )
				{
					nextController();
				}
			}

			INPUT().consume();
		}

		/**
		 * @brief Controller insertion mode.
		 */
		enum struct CTRL_INSERTION_MODE
		{
			FRONT,
			BACK
		};

		/**
		 * @brief Add a controller to the queue.
		 */
		template<Controller::ConceptController C, typename... Args>
		void addController( const CTRL_INSERTION_MODE p_mode, Args &&... p_args )
		{
			// Create.
			auto controller = std::make_unique<C>( std::forward<Args>( p_args )... );

			// Connect.
			UpdateDelegate d;
			d.template connect<&C::update>( controller.get() );

			// Remove previous instance of same type.
			// TODO: make this facultative.
			Hash hash = Util::hash<C>();
			std::erase_if( _controllers, [ hash ]( const Entry & e ) { return e.hash == hash; } );

			// Push.
			Entry entry { std::move( controller ), std::move( d ), hash };
			if ( p_mode == CTRL_INSERTION_MODE::FRONT )
			{
				// Stop previous.
				if ( not _controllers.empty() )
				{
					_controllers.front().ctrl->stop();
				}

				_controllers.emplace_front( std::move( entry ) );
			}
			else
			{
				_controllers.emplace_back( std::move( entry ) );
			}
		}

		/**
		 * @brief Set controller (erase queue).
		 */
		template<Controller::ConceptController C, typename... Args>
		void setController( Args &&... p_args )
		{
			_controllers.clear();
			addController<C>( CTRL_INSERTION_MODE::BACK, std::forward<Args>( p_args )... );
		}

		/**
		 * @brief Go to next controller.
		 */
		inline void nextController()
		{
			if ( not _controllers.empty() )
			{
				_controllers.pop_front();
			}
		}

	  private:
		/**
		 * @brief Camera entity.
		 */
		const Entity _entity;

		/**
		 * @brief Controller running definition.
		 */
		struct Entry
		{
			std::unique_ptr<Controller::IController> ctrl;
			UpdateDelegate							 update;
			Hash									 hash;
		};

		/**
		 * @brief Controllers queue (front is current ).
		 */
		std::deque<Entry> _controllers;

		/**
		 * @brief On update controller setting.
		 */
		void _onControllerSetting();

		/**
		 * @brief On update camera.
		 */
		void _onUpdateCamera();

	}; // namespace VTX::App::Pass
} // namespace VTX::App::Pass

#endif
