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

	void Picker::_onMouseLeftClick( const uint p_x, const uint p_y )
	{
		Vec2i ids = VTXApp::get().getMainWindow().getOpenGLWidget().getPickedIds( p_x, p_y );

		// Clear selection if not CTRL.
		if ( _isModifierExclusive( ModifierFlag::Control ) == false )
		{
			VTX_ACTION(
				new Action::Selection::ClearSelection( Selection::SelectionManager::get().getSelectionModel() ) );
		}

		// If something clicked.
		if ( ids.x != Model::ID_UNKNOWN )
		{
			ID::VTX_ID typeId = MVC::MvcManager::get().getModelTypeID( ids.x );
			// Already selected.
			if ( Selection::SelectionManager::get().getSelectionModel().isModelSelected(
					 MVC::MvcManager::get().getModel<Model::BaseModel>( ids.x ) )
				 && ( ids.y != Model::ID_UNKNOWN
						  ? Selection::SelectionManager::get().getSelectionModel().isModelSelected(
							  MVC::MvcManager::get().getModel<Model::BaseModel>( ids.y ) )
						  : true ) )
			{
				// Remove from selection.
				if ( _isModifierExclusive( ModifierFlag::Control ) )
				{
					// Residue.
					if ( typeId == ID::Model::MODEL_RESIDUE )
					{
						VTX_ACTION( new Action::Selection::UnselectResidue(
							Selection::SelectionManager::get().getSelectionModel(),
							MVC::MvcManager::get().getModel<Model::Residue>( ids.x ) ) );
					}
					// Bond.
					else if ( ids.y != Model::ID_UNKNOWN )
					{
						VTX_ACTION( new Action::Selection::UnselectAtom(
							Selection::SelectionManager::get().getSelectionModel(),
							{ &MVC::MvcManager::get().getModel<Model::Atom>( ids.x ),
							  &MVC::MvcManager::get().getModel<Model::Atom>( ids.y ) } ) );
					}
					// Atom.
					else
					{
						VTX_ACTION( new Action::Selection::UnselectAtom(
							Selection::SelectionManager::get().getSelectionModel(),
							MVC::MvcManager::get().getModel<Model::Atom>( ids.x ) ) );
					}
				}
			}
			// Add to selection.
			else
			{
				// Residue.
				if ( typeId == ID::Model::MODEL_RESIDUE )
				{
					VTX_ACTION(
						new Action::Selection::SelectResidue( Selection::SelectionManager::get().getSelectionModel(),
															  MVC::MvcManager::get().getModel<Model::Residue>( ids.x ),
															  _isModifierExclusive( ModifierFlag::Control ) ) );
				}
				// Bond.
				else if ( ids.y != Model::ID_UNKNOWN )
				{
					VTX_ACTION(
						new Action::Selection::SelectAtom( Selection::SelectionManager::get().getSelectionModel(),
														   { &MVC::MvcManager::get().getModel<Model::Atom>( ids.x ),
															 &MVC::MvcManager::get().getModel<Model::Atom>( ids.y ) },
														   _isModifierExclusive( ModifierFlag::Control ) ) );
				}
				// Atom.
				else
				{
					VTX_ACTION(
						new Action::Selection::SelectAtom( Selection::SelectionManager::get().getSelectionModel(),
														   MVC::MvcManager::get().getModel<Model::Atom>( ids.x ),
														   _isModifierExclusive( ModifierFlag::Control ) ) );
				}
			}
		}
	}

	void Picker::_onMouseLeftDoubleClick( const uint p_x, const uint p_y ) { VTX_DEBUG( "dbl click" ); }

} // namespace VTX::Controller
