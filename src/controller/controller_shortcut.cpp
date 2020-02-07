#include "controller_shortcut.hpp"
#include "../action/action_checkpoint_create.hpp"
#include "../define.hpp"
#include "../vtx_app.hpp"

namespace VTX
{
	namespace Controller
	{
		void ControllerShortcut::_handleKeyPressedEvent( const SDL_Scancode & p_key )
		{
			if ( p_key == SDL_SCANCODE_F1 )
			{
				VTXApp::get().action( new Action::ActionCheckpointCreate( *VTXApp::get().getScene().getPaths()[ 0 ],
																		  VTXApp::get().getScene().getCamera() ) );
			}
			if ( p_key == SDL_SCANCODE_F10 ) { VTXApp::get().TMP_RENDER = !VTXApp::get().TMP_RENDER; }
		}

	} // namespace Controller
} // namespace VTX
