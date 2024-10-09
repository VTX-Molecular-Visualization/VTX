#ifndef ___VTX_APP_VTX_APP___
#define ___VTX_APP_VTX_APP___

#include "app/application/_fwd.hpp"
#include "app/tool/base_tool.hpp"
#include "args.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <util/callback.hpp>
#include <util/chrono.hpp>
#include <util/exceptions.hpp>
#include <util/monitoring/stats.hpp>
#include <vector>

namespace VTX::App
{

	class VTXApp
	{
	  public:
		virtual ~VTXApp() = default;

		static void	 init();
		virtual void start( const Args & );
		static void	 update( const float p_deltaTime, const float p_elapsedTime );
		static void	 stop();

		inline static void addTool( Tool::BaseTool * const p_tool ) { _tools.push_back( p_tool ); }

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
		inline static Util::Callback<>		onStop;

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
} // namespace VTX::App

namespace VTX
{
	//
	using APP = App::VTXApp;
} // namespace VTX

#endif
