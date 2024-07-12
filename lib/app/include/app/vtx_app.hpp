#ifndef ___VTX_APP_VTX_APP___
#define ___VTX_APP_VTX_APP___

#include "app/application/_fwd.hpp"
#include "app/core/monitoring/stats.hpp"
#include "app/mode/base_mode.hpp"
#include "args.hpp"
#include "core/system/system_handler.hpp"
#include <memory>
#include <string>
#include <util/callback.hpp>
#include <util/chrono.hpp>
#include <util/exceptions.hpp>
// #include <util/generic/base_static_singleton.hpp>
#include <vector>

namespace VTX::App
{
	class VTXApp
	{
		// friend class Util::Generic::UniqueInstance<VTXApp>;

	  private:
		inline static const Util::Hashing::Hash SCENE_KEY = Util::Hashing::hash( "SCENE" );

	  public:
		VTXApp();
		virtual ~VTXApp();

		virtual void start( const Args & );
		void		 update( const float p_elapsedTime = 0 );
		void		 stop();

		inline const Core::System::SystemHandler & getSystemHandler() const { return *_systemHandlerPtr; };
		inline Core::System::SystemHandler &	   getSystemHandler() { return *_systemHandlerPtr; };

		inline std::shared_ptr<Core::System::SystemHandler> & getSystemHandlerPtr() { return _systemHandlerPtr; };
		inline void referenceSystemHandler( std::shared_ptr<Core::System::SystemHandler> p_systemHandlerPtr )
		{
			_systemHandlerPtr = p_systemHandlerPtr;
		};

		const Core::Monitoring::Stats & getStats() const { return _stats; }
		Core::Monitoring::Stats &		getStats() { return _stats; }

		Application::Scene &	   getScene();
		const Application::Scene & getScene() const;

		inline Mode::BaseMode &		  getCurrentMode() { return *_currentMode; }
		inline const Mode::BaseMode & getCurrentMode() const { return *_currentMode; }

		inline static Util::Callback<> onStart;
		inline static Util::Callback<> onStartUI;

		inline static Util::Callback<float> onPreUpdate;
		inline static Util::Callback<float> onUpdate;
		inline static Util::Callback<float> onLateUpdate;
		inline static Util::Callback<float> onPostUpdate;

		inline static Util::Callback<float> onPreRender;
		inline static Util::Callback<float> onRender;
		inline static Util::Callback<float> onPostRender;

		inline static Util::Callback<> onEndOfFrameOneShot;

		inline static Util::Callback<> onStop;

	  protected:
	  private:
		inline static Util::Chrono _tickChrono;

		inline static std::shared_ptr<Core::System::SystemHandler> _systemHandlerPtr
			= std::make_shared<Core::System::SystemHandler>();

		inline static std::unique_ptr<Mode::BaseMode> _currentMode;
		inline static std::string					  _currentModeKey = "MODE_VISUALIZATION";

		inline static Core::Monitoring::Stats _stats;

		void _handleArgs( const std::vector<std::string> & );
		void _update( const float p_elapsedTime );
		void _stop();
	};

	// Convenient accessors
	Application::Scene & SCENE();
	Mode::BaseMode &	 MODE();
} // namespace VTX::App

namespace VTX
{
	// inline App::VTXApp & APP() { return Util::Generic::UniqueInstance<App::VTXApp>::get(); }
	using APP = App::VTXApp;
} // namespace VTX

#endif
