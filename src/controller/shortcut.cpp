#include "shortcut.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "action/viewpoint.hpp"
#include "define.hpp"
#include "state/visualization.hpp"
#include "tool/logger.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Controller
	{
		void Shortcut::_handleKeyDownEvent( const int & p_key )
		{
			// TODO: link with action and UI button.
			switch ( p_key )
			{
			case Qt::Key_F1: VTX_ACTION( new Action::Main::ToggleCameraController() ); break;
			case Qt::Key_F2:
				VTX_ACTION(
					new Action::Setting::ChangeRepresentation( Generic::REPRESENTATION( ( (uint)VTX_SETTING().representation + 1 ) % (uint)Generic::REPRESENTATION::COUNT ) ) );
				break;
			case Qt::Key_F3: VTX_ACTION( new Action::Setting::ChangeColorMode( Generic::COLOR_MODE( ( (uint)VTX_SETTING().colorMode + 1 ) % 4 ) ) ); break;
			case Qt::Key_F4: VTX_ACTION( new Action::Setting::ChangeShading( Renderer::SHADING( ( (uint)VTX_SETTING().shading + 1 ) % (uint)Renderer::SHADING::COUNT ) ) ); break;
			case Qt::Key_F5:
				VTX_ACTION(
					new Action::Viewpoint::Create( *VTXApp::get().getScene().getPaths()[ 0 ],
												   VTXApp::get().getScene().getCamera(),
												   VTXApp::get().getStateMachine().getItem<State::Visualization>( ID::State::VISUALIZATION )->getCurrentCameraController() ) );
				break;

			case Qt::Key_F10: VTX_ACTION( new Action::Setting::ActiveRenderer( !VTX_SETTING().activeRenderer ) ); break;

			case Qt::Key_F11: VTX_ACTION( new Action::Setting::ChangeRenderMode( Renderer::MODE( ( (uint)VTX_SETTING().mode + 1 ) % (uint)Renderer::MODE::COUNT ) ) ); break;
			default: break;
			}
		}

	} // namespace Controller
} // namespace VTX
