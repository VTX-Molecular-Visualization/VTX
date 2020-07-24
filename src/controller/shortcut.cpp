#include "shortcut.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "action/viewpoint.hpp"
#include "define.hpp"
#include "state/visualization.hpp"
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
			case SDL_SCANCODE_F1: VTX_ACTION( new Action::Main::ChangeCameraController() ); break;
			case SDL_SCANCODE_F2:
				VTX_ACTION( new Action::Setting::ChangeRepresentation( Generic::REPRESENTATION(
					( (uint)VTX_SETTING().representation + 1 ) % (uint)Generic::REPRESENTATION::COUNT ) ) );
				break;
			case SDL_SCANCODE_F3:
				VTX_ACTION( new Action::Setting::ChangeColorMode(
					Generic::COLOR_MODE( ( (uint)VTX_SETTING().colorMode + 1 ) % (uint)Generic::COLOR_MODE::COUNT ) ) );
				break;
			case SDL_SCANCODE_F4:
				VTX_ACTION( new Action::Setting::ChangeShading(
					Renderer::SHADING( ( (uint)VTX_SETTING().shading + 1 ) % (uint)Renderer::SHADING::COUNT ) ) );
				break;
			case SDL_SCANCODE_F5:
				VTX_ACTION(
					new Action::Viewpoint::Create( *VTXApp::get().getScene().getPaths()[ 0 ],
												   VTXApp::get().getScene().getCamera(),
												   VTXApp::get()
													   .getStateMachine()
													   .getItem<State::Visualization>( ID::State::VISUALIZATION )
													   ->getCurrentController() ) );
				break;

			case SDL_SCANCODE_F10:
				VTX_ACTION( new Action::Setting::ActiveRenderer( !VTX_SETTING().activeRenderer ) );
				break;

			case SDL_SCANCODE_F11:
				VTX_ACTION( new Action::Setting::ChangeRenderMode(
					Renderer::MODE( ( (uint)VTX_SETTING().mode + 1 ) % (uint)Renderer::MODE::COUNT ) ) );
			default: break;
			}
		}

	} // namespace Controller
} // namespace VTX
