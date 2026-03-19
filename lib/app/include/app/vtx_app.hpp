#ifndef __VTX_APP_VTX_APP__
#define __VTX_APP_VTX_APP__

#include "app/ecs.hpp"
#include "app/pipeline.hpp"
#include "app/tool/base_tool.hpp"
#include "args.hpp"
#include <functional>
#include <memory>
#include <vector>

namespace VTX::App
{
	/**
	 * @brief Available command line arguments.
	 */
	constexpr Arg ARG_DEBUG		  = "-debug";
	constexpr Arg ARG_NO_GUI	  = "-no-gui";
	constexpr Arg ARG_NO_GRAPHICS = "-no-graphics";
	constexpr Arg ARG_NO_UPDATE	  = "-no-update";

	/**
	 * @brief Main application class.
	 */
	class VTXApp
	{
	  public:
		/**
		 * @brief Constructs a VTXApp object with the specified arguments.
		 */
		VTXApp( const Args & p_args );

		/**
		 * @brief Virtual destructor.
		 */
		virtual ~VTXApp();

		/**
		 * @brief Start the application, can be overidden by gui.
		 */
		virtual void start();

		/**
		 * @brief Main loop update function (called from gui thread).
		 */
		inline void update( const float p_deltaTime, const float p_elapsedTime )
		{
			_pipeline.process( ECS::getCtx<Util::Monitoring::Stats>(), p_deltaTime, p_elapsedTime );
		}

		/**
		 * @brief Register a tool from another Conan package.
		 */
		using ToolFactory = std::function<std::unique_ptr<Tool::BaseTool>()>;
		inline void addToolFactory( const ToolFactory p_factory )
		{
			_toolFactories.push_back( std::move( p_factory ) );
		}

	  protected:
		/**
		 * @brief Start application services (logger, event hub, etc.).
		 */
		void _startServices();

		/**
		 * @brief Create initial entities (camera, scene root, presets, etc.) and add main passes to the pipeline.
		 */
		void _createInitialEntities();

		/**
		 * @brief Finish the startup process (apply settings, trigger events, etc.).
		 */
		void _finishStartup();

		/**
		 * @brief Instantiate tools from registered factories.
		 */
		void _instantiateTools();

	  private:
		/**
		 * @brief Main ECS registry.
		 */
		ECS::Registry _registry;

		/**
		 * @brief Application pipeline (main loop).
		 */
		Pipeline _pipeline;

		/**
		 * @brief Tools to create.
		 */
		std::vector<ToolFactory> _toolFactories;

		/**
		 * @brief Instantiated tools.
		 */
		std::vector<std::unique_ptr<Tool::BaseTool>> _tools;

		// TODO: redo.
		void _handleArgs( const Args & p_args );
	};

} // namespace VTX::App

#endif
