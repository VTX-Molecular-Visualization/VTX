#ifndef __VTX_APP_VTX_APP__
#define __VTX_APP_VTX_APP__

#include "app/arguments.hpp"
#include "app/ecs.hpp"
#include "app/pipeline.hpp"
#include "app/tool/base_tool.hpp"
#include <memory>
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
		 * @brief Main loop update function (called from gui thread).
		 */
		inline void update( const float p_deltaTime, const float p_elapsedTime )
		{
			_pipeline.process( ECS::getCtx<Util::Monitoring::Stats>(), p_deltaTime, p_elapsedTime );
		}

		/**
		 * @brief Add a tool.
		 */
		template<typename T>
		inline void addTool()
		{
			_tools.emplace_back( std::make_unique<T>() );
		}

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
		inline std::vector<std::unique_ptr<Tool::BaseTool>> & getTools() { return _tools; }

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
	};

} // namespace VTX::App

#endif
