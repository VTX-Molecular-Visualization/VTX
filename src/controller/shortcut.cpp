#include "shortcut.hpp"
#include "action/dev.hpp"
#include "action/main.hpp"
#include "action/selection.hpp"
#include "action/setting.hpp"
#include "action/viewpoint.hpp"
#include "define.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "model/representation/representation_library.hpp"
#include "model/selection.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
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
			bool shortcutEaten = false;

			switch ( _currentGroup )
			{
			case SHORTCUTGROUP::RENDER: shortcutEaten = _handleRenderShortcut( p_key ); break;
			case SHORTCUTGROUP::SCENE: shortcutEaten = _handleSceneShortcut( p_key ); break;
			}

			if ( !shortcutEaten )
			{
				_handleDefaultShortcut( p_key );
			}
		}

		bool Shortcut::_handleDefaultShortcut( const ScanCode & p_key )
		{
			bool shortcutEaten = false;

			const Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();

			switch ( p_key )
			{
			case ScanCode::F6:
				VTX_ACTION( new Action::Setting::RestoreLayout() );
				shortcutEaten = true;
				break;
#ifndef VTX_PRODUCTION
			case ScanCode::F8:
				VTX_ACTION( new Action::Dev::CompileShaders() );
				shortcutEaten = true;
				break;

			case ScanCode::F9:
				VTX_ACTION( new Action::Setting::ActiveRenderer( !VTX_SETTING().getActivateRenderer() ) );
				shortcutEaten = true;
				break;
#endif
			case ScanCode::Delete:
				if ( !Selection::SelectionManager::get().getSelectionModel().isEmpty() )
				{
					VTX_ACTION(
						new Action::Selection::Delete( Selection::SelectionManager::get().getSelectionModel() ) );
				}
				shortcutEaten = true;
				break;

			case ScanCode::S:
				if ( modifiers == Qt::ControlModifier )
				{
					VTX_ACTION( new Action::Main::Save( VTXApp::get().getScenePathData().getCurrentPath() ) );
					shortcutEaten = true;
				}
				else if ( modifiers == ( Qt::ControlModifier | Qt::ShiftModifier ) )
				{
					UI::Dialog::openSaveSessionDialog();
					shortcutEaten = true;
				}
				break;

			case ScanCode::A:
				if ( modifiers == Qt::ControlModifier )
				{
					VTX_ACTION( new Action::Selection::SelectAll() );
					shortcutEaten = true;
				}
				break;

			case ScanCode::D:
				if ( modifiers == Qt::ControlModifier )
				{
					Model::Selection & selectionModel = Selection::SelectionManager::get().getSelectionModel();
					if ( !selectionModel.isEmpty() )
						VTX_ACTION( new Action::Selection::Copy( selectionModel ) );
					shortcutEaten = true;
				}
				break;

			case ScanCode::E:
				if ( modifiers & Qt::ControlModifier )
				{
					Model::Selection & selectionModel = Selection::SelectionManager::get().getSelectionModel();
					if ( !selectionModel.isEmpty() )
						VTX_ACTION( new Action::Selection::Extract( selectionModel ) );
					shortcutEaten = true;
				}
				break;

			default: break;
			}

			return shortcutEaten;
		}
		bool Shortcut::_handleRenderShortcut( const ScanCode & p_key )
		{
			bool shortcutEaten = false;

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
				shortcutEaten = true;
				break;
			case ScanCode::F5:
				VTX_ACTION( new Action::Main::Snapshot(
					Worker::Snapshoter::MODE::GL,
					Util::Filesystem::getSnapshotsPath( Util::Time::getTimestamp() + ".png" ) ) );
				shortcutEaten = true;
				break;
#ifndef VTX_PRODUCTION
			case ScanCode::F2:
				VTX_ACTION( new Action::Setting::ChangeDefaultRepresentation(
					( ( VTX_SETTING().getDefaultRepresentationIndex() + 1 )
					  % Model::Representation::RepresentationLibrary::get().getRepresentationCount() ) ) );
				shortcutEaten = true;
				break;
			case ScanCode::F3:
				if ( VTXApp::get().getScene().getMolecules().size() > 0 )
				{
					Generic::COLOR_MODE colorMode
						= VTXApp::get().getScene().getMolecules().begin()->first->getRepresentation()->getColorMode();
					VTX_ACTION( new Action::Setting::ChangeColorMode(
						Generic::COLOR_MODE( ( uint( colorMode ) + 1 ) % uint( Generic::COLOR_MODE::INHERITED ) ) ) );
				}
				shortcutEaten = true;
				break;
			case ScanCode::F4:
				VTX_ACTION( new Action::Setting::ChangeShading( Renderer::SHADING(
					( (uint)VTX_RENDER_EFFECT().getShading() + 1 ) % (uint)Renderer::SHADING::COUNT ) ) );
				shortcutEaten = true;
				break;
#endif
			}

			return shortcutEaten;
		}
		bool Shortcut::_handleSceneShortcut( const ScanCode & p_key )
		{
			bool shortcutEaten = false;

			/*switch ( p_key )
			{
			default: break;
			}*/

			return shortcutEaten;
		}

	} // namespace Controller
} // namespace VTX
