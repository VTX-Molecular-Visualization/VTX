#ifndef __VTX_APP_VTX_APP__
#define __VTX_APP_VTX_APP__

#include "app/application/_fwd.hpp"
#include "app/tool/base_tool.hpp"
#include "args.hpp"
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
		virtual ~VTXApp() = default;

		/**
		 * @brief Initialize the application.
		 */
		static void init();

		/**
		 * @brief Start the application.
		 * @param the command line arguments.
		 */
		virtual void start( const Args & );

		/**
		 * @brief Main loop update function.
		 * @param p_deltaTime the time since the last frame. In milliseconds ?
		 * @param p_elapsedTime the time since the start of the application.
		 */
		static void update( const float p_deltaTime, const float p_elapsedTime );

		/**
		 * @brief Stop the application.
		 */
		static void stop();

		inline static void addTool( Tool::BaseTool * const p_tool ) { _tools.push_back( p_tool ); }

		// TODO: get entity from ecs directly?
		inline static Application::Scene & getScene() { return *_scene; }

		// Main loop calllbacks.
		inline static Util::Callback<> onStart;

		// inline static Util::Callback<float> onPreUpdate;
		inline static Util::Callback<float, float> onUpdate;
		// inline static Util::Callback<float> onLateUpdate;
		inline static Util::Callback<float> onPostUpdate;
		// inline static Util::Callback<float> onPreRender;
		// inline static Util::Callback<float> onRender;
		inline static Util::Callback<float> onPostRender;
		inline static Util::Callback<>		onEndOfFrameOneShot;
		inline static Util::Callback<>
			onStop; // TODO : Clarify : Is this the application stop ? So it will execute once ?

		// Progress dialog callbacks.
		inline static Util::Callback<std::string_view> onStartBlockingOperation;
		inline static Util::Callback<float>			   onUpdateBlockingOperation;
		inline static Util::Callback<>				   onEndBlockingOperation;

		// TODO: thread callbacks?

	  protected:
		inline static std::vector<Tool::BaseTool *> _tools;

	  private:
		inline static Application::Scene * _scene;

		static void _handleArgs( const Args & p_args );
		static void _update( const float p_deltaTime, const float p_elapsedTime );
	};

	Application::Scene &	  SCENE();
	Util::Monitoring::Stats & STATS();
	Updater &				  UPDATER();
} // namespace VTX::App

namespace VTX
{
	//
	using APP = App::VTXApp;
} // namespace VTX

#endif
