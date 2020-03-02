#include "shortcut.hpp"
#include "action/active_renderer.hpp"
#include "action/new.hpp"
#include "action/viewpoint_create.hpp"
#include "define.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Controller
	{
		void Shortcut::_handleKeyPressedEvent( const SDL_Scancode & p_key )
		{
			// TODO: link with action and UI button.
			switch ( p_key )
			{
			case SDL_SCANCODE_F1:
				VTXApp::get().getActionManager().execute( new Action::ViewpointCreate(
					*VTXApp::get().getScene().getPaths()[ 0 ], VTXApp::get().getScene().getCamera() ) );
				break;
			case SDL_SCANCODE_F2: VTXApp::get().getActionManager().execute( new Action::New() ); break;
			case SDL_SCANCODE_F10:
				VTXApp::get().getActionManager().execute( new Action::ActiveRenderer( !Setting::Rendering::isActive ) );
				break;
			default: break;
			}
		}

	} // namespace Controller
} // namespace VTX
