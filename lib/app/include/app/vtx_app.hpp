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
#include <util/generic/base_static_singleton.hpp>
#include <vector>

namespace VTX::App
{
	class VTXApp
	{
		friend class Util::Generic::UniqueInstance<VTXApp>;

	  private:
		inline static const Util::Hashing::Hash SCENE_KEY = Util::Hashing::hash( "SCENE" );

	  public:
		VTXApp( std::initializer_list<int> ) = delete;
		VTXApp( const VTXApp & )			 = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
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

		Util::Callback<> onStart;
		Util::Callback<> onStartUI;

		Util::Callback<float> onPreUpdate;
		Util::Callback<float> onUpdate;
		Util::Callback<float> onLateUpdate;
		Util::Callback<float> onPostUpdate;

		Util::Callback<float> onPreRender;
		Util::Callback<float> onRender;
		Util::Callback<float> onPostRender;

		Util::Callback<> onEndOfFrameOneShot;

		Util::Callback<> onStop;

	  protected:
		VTXApp();

	  private:
		Util::Chrono								 _tickChrono = Util::Chrono();
		std::shared_ptr<Core::System::SystemHandler> _systemHandlerPtr
			= std::make_shared<Core::System::SystemHandler>();

		std::unique_ptr<Mode::BaseMode> _currentMode;
		std::string						_currentModeKey = "MODE_VISUALIZATION";

		Core::Monitoring::Stats _stats;

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
	inline App::VTXApp & APP() { return Util::Generic::UniqueInstance<App::VTXApp>::get(); }
} // namespace VTX

#endif
