#include "shortcut.hpp"
#include "action/active_renderer.hpp"
#include "action/change_camera_controller.hpp"
#include "action/change_color_mode.hpp"
#include "action/change_representation.hpp"
#include "action/change_shading.hpp"
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
			case SDL_SCANCODE_F1: VTX_ACTION( new Action::ChangeCameraController() ); break;
			case SDL_SCANCODE_F2:
				VTX_ACTION( new Action::ChangeRepresentation( Generic::REPRESENTATION(
					( (uint)VTX_SETTING().representation + 1 ) % (uint)Generic::REPRESENTATION::COUNT ) ) );
				break;
			case SDL_SCANCODE_F3:
				VTX_ACTION( new Action::ChangeColorMode(
					Generic::COLOR_MODE( ( (uint)VTX_SETTING().colorMode + 1 ) % (uint)Generic::COLOR_MODE::COUNT ) ) );
				break;
			case SDL_SCANCODE_F4:
				VTX_ACTION( new Action::ChangeShading(
					Renderer::SHADING( ( (uint)VTX_SETTING().shading + 1 ) % (uint)Renderer::SHADING::COUNT ) ) );
				break;
			case SDL_SCANCODE_F5:
				VTX_ACTION( new Action::ViewpointCreate( *VTXApp::get().getScene().getPaths()[ 0 ],
														 VTXApp::get().getScene().getCamera() ) );
				break;

			case SDL_SCANCODE_F10: VTX_ACTION( new Action::ActiveRenderer( !VTX_SETTING().activeRenderer ) ); break;
			default: break;
			}
		}

	} // namespace Controller
} // namespace VTX
