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

	void Picker::_handleMouseButtonDownEvent( const QMouseEvent & p_event )
	{
		if ( _mouseHoveringRenderWidget() )
		{
			uint id
				= VTXApp::get().getMainWindow().getOpenGLWidget().getPickingId( p_event.pos().x(), p_event.pos().y() );
			VTX_DEBUG( std::to_string( id ) );

			if ( id == 0 )
			{
				if ( _isModifierExclusive( ModifierFlag::Control ) == false )
				{
					VTX_ACTION( new Action::Selection::ClearSelection(
						Selection::SelectionManager::get().getSelectionModel() ) );
				}
			}
			else
			{
				Model::Atom & atom = MVC::MvcManager::get().getModel<Model::Atom>( id );
				VTX_ACTION( new Action::Selection::SelectAtom( Selection::SelectionManager::get().getSelectionModel(),
															   atom,
															   _isModifierExclusive( ModifierFlag::Control ) ) );
			}
		}
	}

} // namespace VTX::Controller
