#include "shortcut.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "action/viewpoint.hpp"
#include "define.hpp"
#include "model/path.hpp"
#include "model/representation/representation_library.hpp"
#include "state/visualization.hpp"
#include "tool/logger.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Controller
	{
		void Shortcut::_handleKeyDownEvent( const ScanCode & p_key )
		{
			// TODO: link with action and UI button.
			switch ( p_key )
			{
			case ScanCode::F1:
				if ( QApplication::keyboardModifiers() & Qt::ControlModifier )
				{
					VTX_ACTION( new Action::Main::ResetCameraController() );
				}
				else
				{
					VTX_ACTION( new Action::Main::ToggleCameraController() );
				}
				break;
			case ScanCode::F2:
				VTX_ACTION( new Action::Setting::ChangeRepresentation(
					( VTX_SETTING().representation + 1 )
					% Model::Representation::RepresentationLibrary::get().getRepresentationCount() ) );
				break;
			case ScanCode::F3:
				VTX_ACTION( new Action::Setting::ChangeColorMode(
					Generic::COLOR_MODE( ( (uint)VTX_SETTING().colorMode + 1 ) % 4 ) ) );
				break;
			case ScanCode::F4:
				VTX_ACTION( new Action::Setting::ChangeShading(
					Renderer::SHADING( ( (uint)VTX_SETTING().shading + 1 ) % (uint)Renderer::SHADING::COUNT ) ) );
				break;
			case ScanCode::F5:
				VTX_ACTION(
					new Action::Viewpoint::Create( *VTXApp::get().getScene().getPaths()[ 0 ],
												   VTXApp::get().getScene().getCamera(),
												   VTXApp::get()
													   .getStateMachine()
													   .getItem<State::Visualization>( ID::State::VISUALIZATION )
													   ->getCurrentCameraController() ) );
				break;
			case ScanCode::F6: VTX_ACTION( new Action::Setting::RestoreLayout() ); break;
			case ScanCode::F10:
				VTX_ACTION( new Action::Setting::ActiveRenderer( !VTX_SETTING().activeRenderer ) );
				break;

			case ScanCode::F11:
				VTX_ACTION( new Action::Setting::ChangeRenderMode(
					Renderer::MODE( ( (uint)VTX_SETTING().mode + 1 ) % (uint)Renderer::MODE::COUNT ) ) );
				break;
			default: break;
			}
		}

	} // namespace Controller
} // namespace VTX
