#include "shortcut.hpp"
#include "../action/active_renderer.hpp"
#include "../action/viewpoint_create.hpp"
#include "../define.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace Controller
	{
		void Shortcut::_handleKeyPressedEvent( const SDL_Scancode & p_key )
		{
			switch ( p_key )
			{
			case SDL_SCANCODE_F1:
				VTXApp::get().action( new Action::ViewpointCreate( *VTXApp::get().getScene().getPaths()[ 0 ],
																   VTXApp::get().getScene().getCamera() ) );
				break;
			case SDL_SCANCODE_F10:
				VTXApp::get().action( new Action::ActiveRenderer( !Setting::Rendering::isActive ) );
				break;
			default: break;
			}
		}

	} // namespace Controller
} // namespace VTX
