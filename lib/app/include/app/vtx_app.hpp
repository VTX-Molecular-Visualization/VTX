#ifndef ___VTX_APP_VTX_APP___
#define ___VTX_APP_VTX_APP___

#include "app/application/_fwd.hpp"
#include "app/core/monitoring/stats.hpp"
#include "app/mode/base_mode.hpp"
#include "app/tool/base_tool.hpp"
#include "args.hpp"
#include "core/system/system_handler.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <util/callback.hpp>
#include <util/chrono.hpp>
#include <util/exceptions.hpp>
#include <vector>

namespace VTX::App
{

	class VTXApp
	{
	  private:
		inline static const Hash SCENE_KEY = Util::hash( "SCENE" );

	  public:
		virtual ~VTXApp() = default;

		static void	 init();
		virtual void start( const Args & );
		static void	 update( const float p_deltaTime, const float p_elapsedTime );
		static void	 stop();

		inline static Core::System::SystemHandler & getSystemHandler() { return *_systemHandler; }
		// inline Core::System::SystemHandler * const getSystemHandlerPtr() { return _systemHandler.get(); }

		/*
		inline void referenceSystemHandler( std::shared_ptr<Core::System::SystemHandler> p_systemHandlerPtr )
		{
			_systemHandlerPtr = p_systemHandlerPtr;
		};
		*/

		inline static Core::Monitoring::Stats & getStats() { return _stats; }
		static Application::Scene &				getScene();
		inline static Mode::BaseMode &			getCurrentMode() { return *_currentMode; }

		inline static void addTool( Tool::BaseTool * const p_tool ) { _tools.push_back( p_tool ); }

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
		inline static Util::Callback<std::string> onStartBlockingOperation;
		inline static Util::Callback<float>		  onUpdateBlockingOperation;
		inline static Util::Callback<>			  onEndBlockingOperation;

		// TODO: thread callbacks?

	  protected:
		inline static std::vector<Tool::BaseTool *> _tools;

	  private:
		inline static std::unique_ptr<Core::System::SystemHandler> _systemHandler
			= std::make_unique<Core::System::SystemHandler>();

		inline static std::unique_ptr<Mode::BaseMode> _currentMode;
		inline static std::string					  _currentModeKey = "MODE_VISUALIZATION";
		inline static Core::Monitoring::Stats		  _stats;

		static void _handleArgs( const Args & p_args );
		static void _update( const float p_deltaTime, const float p_elapsedTime );
	};

	// Convenient accessors
	Application::Scene &	  SCENE();
	Mode::BaseMode &		  MODE();
	Core::Monitoring::Stats & STATS();
} // namespace VTX::App

namespace VTX
{
	using APP = App::VTXApp;
} // namespace VTX

#endif
