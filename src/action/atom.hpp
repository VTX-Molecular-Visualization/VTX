#ifndef __VTX_ACTION_ATOM__
#define __VTX_ACTION_ATOM__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "model/atom.hpp"
#include "model/generated_molecule.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "util/molecule.hpp"
#include "visible.hpp"
#include "vtx_app.hpp"

namespace VTX::Action::Atom
{
	class ChangeColor : public BaseAction
	{
	  public:
		explicit ChangeColor( Model::Atom & p_atom, const Color::Rgb & p_color ) : _atom( p_atom ), _color( p_color ) {}

		virtual void execute() override
		{
			//_atom.setColor( _color );
			//_atom.getMoleculePtr()->setColorMode();
		}

	  private:
		Model::Atom &	 _atom;
		const Color::Rgb _color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( Model::Atom & p_atom, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_atom, p_mode )
		{
		}

		virtual void execute() override
		{
			// _visibles will always has a size of 1, we can perform computeRepresentationTargets in the loop
			for ( Generic::BaseVisible * const visible : _visibles )
			{
				Model::Atom & atom			= *( (Model::Atom *)visible );
				const bool	  newVisibility = _getVisibilityBool( atom );

				atom.setVisible( newVisibility );

				atom.getMoleculePtr()->computeRepresentationTargets();
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}
	};

	class Orient : public BaseAction
	{
	  public:
		explicit Orient( Model::Atom & p_atom ) : _atom( p_atom ) {}

		virtual void execute() override
		{
			VTXApp::get()
				.getStateMachine()
				.getItem<State::Visualization>( ID::State::VISUALIZATION )
				->orientCameraController( _atom.getWorldAABB() );
		}

	  private:
		Model::Atom & _atom;
	};

	class Delete : public BaseAction
	{
	  public:
		explicit Delete( Model::Atom & p_atom ) : _atom( p_atom ) {}

		virtual void execute() override
		{
			VTX::Selection::SelectionManager::get().getSelectionModel().unselectAtom( _atom );

			Model::Molecule * const molecule = _atom.getMoleculePtr();
			molecule->removeAtom( _atom.getIndex() );

			if ( molecule->isEmpty() )
			{
				VTXApp::get().getScene().removeMolecule( molecule );
				MVC::MvcManager::get().deleteModel( molecule );
			}
			else
			{
				molecule->refreshBondsBuffer();
			}

			VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		Model::Atom & _atom;
	};

	class Copy : public BaseAction
	{
	  public:
		explicit Copy( const Model::Atom & p_target ) : _target( p_target ) {}
		virtual void execute() override
		{
			Model::GeneratedMolecule * generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->copyFromAtom( _target );

			VTX_EVENT( new Event::VTXEventPtr<Model::Molecule>( Event::Global::MOLECULE_CREATED, generatedMolecule ) );

			const float offset = generatedMolecule->getAABB().radius() + _target.getAABB().radius()
								 + VTX::Setting::COPIED_MOLECULE_OFFSET;
			generatedMolecule->setTranslation( VTX::Vec3f( offset, 0, 0 ) );

			VTXApp::get().getScene().addMolecule( generatedMolecule );
		}

	  private:
		const Model::Atom & _target;
	};

	class Extract : public BaseAction
	{
	  public:
		explicit Extract( const Model::Atom & p_target ) : _target( p_target ) {}
		virtual void execute() override
		{
			Model::GeneratedMolecule * const generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->extractAtom( _target );

			VTX_EVENT( new Event::VTXEventPtr<Model::Molecule>( Event::Global::MOLECULE_CREATED, generatedMolecule ) );

			VTXApp::get().getScene().addMolecule( generatedMolecule );

			VTX::Selection::SelectionManager::get().getSelectionModel().clear();
		}

	  private:
		const Model::Atom & _target;
	};
} // namespace VTX::Action::Atom
#endif
