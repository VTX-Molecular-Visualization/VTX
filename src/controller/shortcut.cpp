#include "shortcut.hpp"
#include "action/dev.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "action/viewpoint.hpp"
#include "define.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "model/representation/representation_library.hpp"
#include "object3d/scene.hpp"
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
			case ScanCode::F6: VTX_ACTION( new Action::Setting::RestoreLayout() ); break;
#ifndef VTX_PRODUCTION
			case ScanCode::F2:
				VTX_ACTION( new Action::Setting::ChangeDefaultRepresentation(
					( ( VTX_SETTING().getDefaultRepresentationIndex() + 1 )
					  % Model::Representation::RepresentationLibrary::get().getRepresentationCount() ) ) );
				break;
			case ScanCode::F3:
				if ( VTXApp::get().getScene().getMolecules().size() > 0 )
				{
					Generic::COLOR_MODE colorMode
						= VTXApp::get().getScene().getMolecules().begin()->first->getRepresentation()->getColorMode();
					VTX_ACTION( new Action::Setting::ChangeColorMode(
						Generic::COLOR_MODE( ( uint( colorMode ) + 1 ) % uint( Generic::COLOR_MODE::INHERITED ) ) ) );
				}
				break;
			case ScanCode::F4:
				VTX_ACTION( new Action::Setting::ChangeShading( Renderer::SHADING(
					( (uint)VTX_RENDER_EFFECT().getShading() + 1 ) % (uint)Renderer::SHADING::COUNT ) ) );
				break;
			case ScanCode::F5:
				VTX_ACTION( new Action::Main::Snapshot(
					Worker::Snapshoter::MODE::GL,
					Util::Filesystem::getSnapshotsPath( Util::Time::getTimestamp() + ".png" ) ) );
				break;
			case ScanCode::F8: VTX_ACTION( new Action::Dev::CompileShaders() ); break;

			case ScanCode::F9:
				VTX_ACTION( new Action::Setting::ActiveRenderer( !VTX_SETTING().getActivateRenderer() ) );
				break;
#endif
			default: break;
			}
		}

	} // namespace Controller
} // namespace VTX
