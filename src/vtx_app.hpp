#ifndef __VTX_APP__
#define __VTX_APP__

#include "event/event.hpp"
#include "object3d/scene.hpp"
#include "renderer/renderer.hpp"
#include "ui/user_interface.hpp"
#include "util/logger.hpp"
#include <thread>
#include <vector>

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

		void							   fireUIEvent( const Event::EVENT_UI, void * const ) const;
		std::shared_ptr<UI::BaseComponent> getUIComponentByType( UI::COMPONENT_TYPE p_type ) const;

	  private:
		static bool				 _isRunning;
		double					 _time = 0.f;
		UI::UserInterface *		 _ui;
		Object3D::Scene *		 _scene;
		Renderer::Renderer *	 _renderer;
		std::vector<std::thread> _threads = std::vector<std::thread>();

		VTXApp();
		VTXApp( const VTXApp & ) = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp();

		void _update();
	};
} // namespace VTX

#endif
