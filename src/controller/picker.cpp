#include "picker.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/logger.hpp"
#include "ui/main_window.hpp"
#include "vtx_app.hpp"

namespace VTX::Controller
{
	void Picker::update( const float & p_deltaTime ) { BaseMouseController::update( p_deltaTime ); }

	void Picker::_handleMouseButtonDownEvent( const QMouseEvent & p_event ) { _deltaMousePosition = Vec2i(); }

	void Picker::_handleMouseButtonUpEvent( const QMouseEvent & p_event )
	{
		if ( _deltaMousePosition != Vec2i() )
		{
			return;
		}

		if ( _mouseHoveringRenderWidget() )
		{
			uint id
				= VTXApp::get().getMainWindow().getOpenGLWidget().getPickingId( p_event.pos().x(), p_event.pos().y() );

			// Clear selection if not CTRL.
			if ( _isModifierExclusive( ModifierFlag::Control ) == false )
			{
				VTX_ACTION(
					new Action::Selection::ClearSelection( Selection::SelectionManager::get().getSelectionModel() ) );
			}

			// If something clicked.
			if ( id != 0 )
			{
				Model::Atom & atom = MVC::MvcManager::get().getModel<Model::Atom>( id );

				// Already selected.
				if ( Selection::SelectionManager::get().getSelectionModel().isModelSelected( atom ) )
				{
					// Remove from selection.
					if ( _isModifierExclusive( ModifierFlag::Control ) )
					{
						VTX_ACTION( new Action::Selection::UnselectAtom(
							Selection::SelectionManager::get().getSelectionModel(), atom, true ) );
					}
				}
				else
				{
					// Add to selection.
					VTX_ACTION(
						new Action::Selection::SelectAtom( Selection::SelectionManager::get().getSelectionModel(),
														   atom,
														   _isModifierExclusive( ModifierFlag::Control ) ) );
				}
			}
		}
	}

} // namespace VTX::Controller
