#ifndef __VTX_APP_VTX_APP__
#define __VTX_APP_VTX_APP__

#include "app/application/_fwd.hpp"
#include "app/ecs.hpp"
#include "app/pipeline.hpp"
#include "app/tool/base_tool.hpp"
#include "args.hpp"
#include <entt/entity/registry.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <util/callback.hpp>
#include <util/chrono.hpp>
#include <util/exceptions.hpp>
#include <vector>

namespace VTX::Util::Monitoring
{
	class Stats;
}

namespace VTX::App
{
	class Updater;

	constexpr Arg ARG_DEBUG		  = "-debug";
	constexpr Arg ARG_NO_GUI	  = "-no-gui";
	constexpr Arg ARG_NO_GRAPHICS = "-no-graphics";
	constexpr Arg ARG_NO_UPDATE	  = "-no-update";

	class VTXApp
	{
	  public:
		VTXApp( const Args & p_args );
		virtual ~VTXApp() = default;

		/**
		 * @brief Initialize the application.
		 */
		void init();

		/**
		 * @brief Start the application.
		 * @param the command line arguments.
		 */
		virtual void start();

		/**
		 * @brief Main loop update function.
		 * @param p_deltaTime the time since the last frame. In milliseconds.
		 * @param p_elapsedTime the time since the start of the application.
		 */
		inline void update( const float p_deltaTime, const float p_elapsedTime )
		{
			_pipeline.process( ECS::getCtx<Util::Monitoring::Stats>(), p_deltaTime, p_elapsedTime );
		}

		/**
		 * @brief Stop the application.
		 */
		void stop();

		inline void addTool( Tool::BaseTool * const p_tool ) { _tools.push_back( p_tool ); }

		//  TODO: get entity from ecs directly?
		inline static Application::Scene & getScene() { return *_scene; }

		// inline static Util::Callback<float, float> onUpdate;

	  protected:
		inline static std::vector<Tool::BaseTool *> _tools;

	  private:
		ECS::Registry _registry;
		Pipeline	  _pipeline;

		inline static Application::Scene * _scene;

		static void _handleArgs( const Args & p_args );
	};

	Application::Scene & SCENE();
	Updater &			 UPDATER();
} // namespace VTX::App

namespace VTX
{
	//
	using APP = App::VTXApp;
} // namespace VTX

#endif
