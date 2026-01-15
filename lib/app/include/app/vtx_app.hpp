#ifndef __VTX_APP_VTX_APP__
#define __VTX_APP_VTX_APP__

#include "app/ecs.hpp"
#include "app/pipeline.hpp"
#include "app/tool/base_tool.hpp"
#include "args.hpp"
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
		~VTXApp(); // TODO : do we still need to be virtual ?

		/**
		 * @brief Start the application, can be overidden by gui.
		 */
		void start();

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
		inline void addTool( Tool::BaseTool * const p_tool ) { _tools.push_back( p_tool ); }

	  protected:
		/**
		 * @brief External tools.
		 */
		std::vector<Tool::BaseTool *> _tools;

	  private:
		/**
		 * @brief Main ECS registry.
		 */
		ECS::Registry _registry;

		/**
		 * @brief Application pipeline (main loop).
		 */
		Pipeline _pipeline;

		// TODO: redo.
		void _handleArgs( const Args & p_args );
	};

} // namespace VTX::App

#endif
