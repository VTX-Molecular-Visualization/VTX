#ifndef __VTX_APP__
#define __VTX_APP__

#include "event/event.hpp"
#include "object3d/scene.hpp"
#include "renderer/renderer.hpp"
#include "ui/user_interface.hpp"
#include "util/logger.hpp"

namespace VTX
{
	class VTXApp final
	{
	  public:
		static VTXApp & get()
		{
			static VTXApp instance;
			return instance;
		}
		static bool isRunning() { return VTXApp::_isRunning; }

		void start();
		void stop();

		template<typename T>
		void fireUIEvent( const Event::EVENT_UI, const T & ) const;

	  private:
		static bool			 _isRunning;
		double				 _time = 0.f;
		UI::UserInterface *	 _ui;
		Object3D::Scene *	 _scene;
		Renderer::Renderer * _renderer;

		VTXApp();
		VTXApp( const VTXApp & ) = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp();

		void _update();
	};
} // namespace VTX

#endif
