#ifndef VTX_APP_VTX_APP_HPP
#define VTX_APP_VTX_APP_HPP

#include "app/arguments.hpp"
#include "app/ecs.hpp"
#include "app/pipeline.hpp"
#include "app/tool/base_tool.hpp"
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <exception>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <util/atomic_callback.hpp>
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/thread/future.hpp>
#include <vector>

namespace VTX::App
{

	/**
	 * @brief Main application class.
	 */
	class VTXApp
	{
	  public:
		/**
		 * @brief Constructs a VTXApp object with the specified arguments.
		 */
		VTXApp( Arguments && p_args );

		/**
		 * @brief Virtual destructor.
		 */
		virtual ~VTXApp();

		/**
		 * @brief Start the application, can be overidden by gui.
		 */
		virtual void start();

		/**
		 * @brief
		 */
		void setFrameScheduler( Util::AtomicCallback::Function, void * p_context );

		/**
		 * @brief Main update.
		 */
		void update() noexcept;

		/**
		 * @brief Wait until a predicate is true.
		 */
		template<std::predicate P>
		bool runUntil( const P & p_predicate, const std::optional<std::chrono::milliseconds> p_timeout = std::nullopt )
		{
			std::optional<std::chrono::steady_clock::time_point> deadline;
			if ( p_timeout.has_value() )
			{
				deadline = std::chrono::steady_clock::now() + *p_timeout;
			}

			while ( not p_predicate() )
			{
				if ( not _waitForPendingUpdates( deadline ) )
				{
					return p_predicate();
				}
				update();
			}
			return true;
		}

		/**
		 * @brief Wait until the application is idle (no pending updates).
		 */
		bool runUntilIdle( std::optional<std::chrono::milliseconds> p_timeout = std::nullopt );

		/**
		 * @brief Wait until a future is ready, while updating the application.
		 */
		template<typename T>
		bool runUntilReady(
			const std::future<T> &						   p_future,
			const std::optional<std::chrono::milliseconds> p_timeout = std::nullopt
		)
		{
			return runUntil( [ &p_future ]() { return Util::Thread::isReady( p_future ); }, p_timeout );
		}

		/**
		 * @brief Add a tool.
		 */
		template<typename T>
		void addTool()
		{ _tools.emplace_back( std::make_unique<T>() ); }

		/**
		 * @brief Start application services (logger, event hub, etc.).
		 */
		void startServices();

		/**
		 * @brief Create initial entities (camera, scene root, presets, etc.) and add main passes to the pipeline.
		 */
		void createInitialEntities();

		/**
		 * @brief Finish the startup process (apply settings, trigger events, etc.).
		 */
		void finishStartup();

		/**
		 * @brief Perform tasks instructed by the arguments.
		 * Submitted files are loaded on the main thread in sequence.
		 */
		void handleArgs();

		/**
		 * @brief Get instantiated tools.
		 */
		std::vector<std::unique_ptr<Tool::BaseTool>> & getTools() noexcept { return _tools; }

	  private:
		/**
		 * @brief Main ECS registry.
		 */
		Registry _registry;

		/**
		 * @brief Application pipeline (main loop).
		 */
		Pipeline _pipeline;

		/**
		 * @brief Instantiated tools.
		 */
		std::vector<std::unique_ptr<Tool::BaseTool>> _tools;

		/**
		 * @brief Main chrono.
		 */
		Util::Chrono _chrono;

		/**
		 * @brief Is idle.
		 */
		bool _idle = true;

		/**
		 * @brief Mutex and condition.
		 */
		std::mutex				_wakeMutex;
		std::condition_variable _wakeCondition;

		/**
		 * @brief Wait for pending updates, with an optional deadline.
		 */
		bool _waitForPendingUpdates( const std::optional<std::chrono::steady_clock::time_point> & p_deadline );

		/**
		 * @brief Wake function for headless mode (CLI).
		 */
		static void _wake( void * p_app ) noexcept;
	};

} // namespace VTX::App

#endif
