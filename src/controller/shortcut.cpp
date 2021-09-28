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
#include <QLocale>

namespace VTX
{
	namespace Controller
	{
		void Shortcut::_handleKeyDownEvent( const Qt::Key & p_key )
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
				shortcutEaten = _handleDefaultShortcut( p_key );
			}

			if ( shortcutEaten )
			{
				Event::EventManager::get().clearKeyboardInputEvent( p_key );
			}
		}

		bool Shortcut::_handleDefaultShortcut( const Qt::Key & p_key )
		{
			bool shortcutEaten = false;

			switch ( p_key )
			{
			case Qt::Key::Key_Escape:
				if ( !Selection::SelectionManager::get().getSelectionModel().isEmpty() )
				{
					VTX_ACTION( new Action::Selection::ClearSelection(
						Selection::SelectionManager::get().getSelectionModel() ) );
				}
				break;
			case Qt::Key::Key_F6:
				VTX_ACTION( new Action::Setting::RestoreLayout() );
				shortcutEaten = true;
				break;
#ifndef VTX_PRODUCTION
			case Qt::Key::Key_F8:
				VTX_ACTION( new Action::Dev::CompileShaders() );
				shortcutEaten = true;
				break;

			case Qt::Key::Key_F9:
				VTX_ACTION( new Action::Setting::ActiveRenderer( !VTX_SETTING().getActivateRenderer() ) );
				shortcutEaten = true;
				break;
#endif
			case Qt::Key::Key_F11:
				if ( VTXApp::get().getMainWindow().windowState() & Qt::WindowStates::enum_type::WindowFullScreen )
					VTX_ACTION( new Action::Setting::WindowMode( UI::WindowMode::Windowed ) );
				else
					VTX_ACTION( new Action::Setting::WindowMode( UI::WindowMode::Fullscreen ) );
				shortcutEaten = true;
				break;

			case Qt::Key::Key_Delete:
				if ( !Selection::SelectionManager::get().getSelectionModel().isEmpty() )
				{
					VTX_ACTION(
						new Action::Selection::Delete( Selection::SelectionManager::get().getSelectionModel() ) );
				}
				shortcutEaten = true;
				break;

			case Qt::Key::Key_S:
				if ( _isModifierExclusive( ModifierFlag::Control ) )
				{
					VTX_ACTION_ENQUEUE( new Action::Main::Save( VTXApp::get().getScenePathData().getCurrentPath() ) );
					shortcutEaten = true;
				}
				else if ( _isModifierExclusive( ModifierFlag( ModifierFlag::Control | ModifierFlag::Shift ) ) )
				{
					// WORKAROUND : Clear buffer before calling dialog (if not, unwanted movement appear)
					Event::EventManager::get().clearKeyboardInputEvent( p_key );
					UI::Dialog::openSaveSessionDialog();
					shortcutEaten = true;
				}
				break;

			case Qt::Key::Key_N:
				if ( _isModifierExclusive( ModifierFlag::Control ) )
				{
					UI::Dialog::createNewSessionDialog();
					shortcutEaten = true;
				}
				break;

			case Qt::Key::Key_O:
				if ( _isModifierExclusive( ModifierFlag::None ) )
				{
					const Model::Selection & selection = Selection::SelectionManager::get().getSelectionModel();

					if ( !selection.isEmpty() )
						VTX_ACTION( new Action::Selection::Orient( selection ) );

					shortcutEaten = true;
				}
				else if ( _isModifierExclusive( ModifierFlag::Control ) )
				{
					UI::Dialog::openLoadSessionDialog();
					shortcutEaten = true;
				}
				break;

			case Qt::Key::Key_Q:
				if ( getKeyboardLayout() == KeyboardLayout::QWERTY && _isModifierExclusive( ModifierFlag::Control ) )
				{
					VTX_ACTION( new Action::Selection::SelectAll() );
					shortcutEaten = true;
				}
				break;

			case Qt::Key::Key_A:
				if ( getKeyboardLayout() == KeyboardLayout::AZERTY && _isModifierExclusive( ModifierFlag::Control ) )
				{
					VTX_ACTION( new Action::Selection::SelectAll() );
					shortcutEaten = true;
				}
				break;

			case Qt::Key::Key_D:
				if ( _isModifierExclusive( ModifierFlag::Control ) )
				{
					Model::Selection & selectionModel = Selection::SelectionManager::get().getSelectionModel();
					if ( !selectionModel.isEmpty() )
						VTX_ACTION( new Action::Selection::Copy( selectionModel ) );
					shortcutEaten = true;
				}
				break;

			case Qt::Key::Key_E:
				if ( _isModifierExclusive( ModifierFlag::Control ) )
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
		bool Shortcut::_handleRenderShortcut( const Qt::Key & p_key )
		{
			bool shortcutEaten = false;

			switch ( p_key )
			{
			case Qt::Key::Key_F1:
				if ( _isModifierExclusive( ModifierFlag::Control ) )
				{
					VTX_ACTION( new Action::Main::ResetCameraController() );
				}
				else if ( _isModifierExclusive( ModifierFlag::None ) )
				{
					VTX_ACTION( new Action::Main::ToggleCameraController() );
				}
				shortcutEaten = true;
				break;
			case Qt::Key::Key_F5:
				VTX_ACTION( new Action::Main::Snapshot(
					Worker::Snapshoter::MODE::GL,
					Util::Filesystem::getSnapshotsPath( Util::Time::getTimestamp() + ".png" ),
					VTX_SETTING().getSnapshotResolution() ) );
				shortcutEaten = true;
				break;
#ifndef VTX_PRODUCTION
			case Qt::Key::Key_F2:
				VTX_ACTION( new Action::Setting::ChangeDefaultRepresentation(
					( ( VTX_SETTING().getDefaultRepresentationIndex() + 1 )
					  % Model::Representation::RepresentationLibrary::get().getRepresentationCount() ) ) );
				shortcutEaten = true;
				break;
			case Qt::Key::Key_F3:
				if ( VTXApp::get().getScene().getMolecules().size() > 0 )
				{
					Generic::COLOR_MODE colorMode
						= VTXApp::get().getScene().getMolecules().begin()->first->getRepresentation()->getColorMode();
					VTX_ACTION( new Action::Setting::ChangeColorMode(
						Generic::COLOR_MODE( ( uint( colorMode ) + 1 ) % uint( Generic::COLOR_MODE::INHERITED ) ) ) );
				}
				shortcutEaten = true;
				break;
			case Qt::Key::Key_F4:
				VTX_ACTION( new Action::Setting::ChangeShading( Renderer::SHADING(
					( (uint)VTX_RENDER_EFFECT().getShading() + 1 ) % (uint)Renderer::SHADING::COUNT ) ) );
				shortcutEaten = true;
				break;
#endif
			}

			return shortcutEaten;
		}
		bool Shortcut::_handleSceneShortcut( const Qt::Key & p_key )
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
