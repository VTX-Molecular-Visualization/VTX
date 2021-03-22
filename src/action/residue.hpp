#ifndef __VTX_ACTION_RESIDUE__
#define __VTX_ACTION_RESIDUE__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "model/chain.hpp"
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

namespace VTX::Action::Residue
{
	class ChangeColor : public BaseAction
	{
	  public:
		explicit ChangeColor( Model::Residue & p_residue, const Color::Rgb & p_color ) :
			_residue( p_residue ), _color( p_color )
		{
		}

		virtual void execute() override
		{
			_residue.setColor( _color );
			_residue.getMoleculePtr()->refreshColors();

			if ( _residue.getMoleculePtr()->getSecondaryStructure().getColorMode()
				 == Model::SecondaryStructure::COLOR_MODE::RESIDUE )
			{
				_residue.getMoleculePtr()->getSecondaryStructure().refreshColors();
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		Model::Residue & _residue;
		const Color::Rgb _color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( Model::Residue & p_residue, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_residue, p_mode )
		{
		}

		virtual void execute() override
		{
			for ( Generic::BaseVisible * const visible : _visibles )
			{
				bool			 newVisibility = _getVisibilityBool( *visible );
				Model::Residue & residue	   = *( static_cast<Model::Residue * const>( visible ) );

				residue.setVisible( newVisibility );

				if ( _mode == VISIBILITY_MODE::ALL || _mode == VISIBILITY_MODE::SOLO )
				{
					for ( uint i = 0; i < residue.getChainPtr()->getResidueCount(); ++i )
					{
						residue.getMoleculePtr()
							->getResidue( residue.getChainPtr()->getIndexFirstResidue() + i )
							->setVisible(
								_mode == VISIBILITY_MODE::ALL
								|| ( _mode == VISIBILITY_MODE::SOLO
									 && residue.getChainPtr()->getIndexFirstResidue() + i == residue.getIndex() ) );
					}
				}

				residue.getMoleculePtr()->computeRepresentationTargets();
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}
	};

	class Orient : public BaseAction
	{
	  public:
		explicit Orient( const Model::Residue & p_residue ) : _residue( p_residue ) {}

		virtual void execute() override
		{
			VTXApp::get()
				.getStateMachine()
				.getItem<State::Visualization>( ID::State::VISUALIZATION )
				->orientCameraController( _residue.getWorldAABB() );
		}

	  private:
		const Model::Residue & _residue;
	};

	class Delete : public BaseAction
	{
	  public:
		explicit Delete( Model::Residue & p_residue ) : _residue( p_residue ) {}

		virtual void execute() override
		{
			VTX::Selection::SelectionManager::get().getSelectionModel().unselectResidue( _residue );

			Model::Molecule * const molecule = _residue.getMoleculePtr();
			molecule->removeResidue( _residue.getIndex() );

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
		Model::Residue & _residue;
	};

	class Copy : public BaseAction
	{
	  public:
		explicit Copy( const Model::Residue & p_target ) : _target( p_target ) {}
		virtual void execute() override
		{
			Model::GeneratedMolecule * generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->copyFromResidue( _target );

			VTX_EVENT( new Event::VTXEventPtr<Model::Molecule>( Event::Global::MOLECULE_CREATED, generatedMolecule ) );

			const float offset = generatedMolecule->getAABB().radius() + _target.getAABB().radius()
								 + VTX::Setting::COPIED_MOLECULE_OFFSET;
			generatedMolecule->setTranslation( VTX::Vec3f( offset, 0, 0 ) );

			VTXApp::get().getScene().addMolecule( generatedMolecule );
		}

	  private:
		const Model::Residue & _target;
	};

	class Extract : public BaseAction
	{
	  public:
		explicit Extract( const Model::Residue & p_target ) : _target( p_target ) {}
		virtual void execute() override
		{
			Model::GeneratedMolecule * const generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->extractResidue( _target );

			VTX_EVENT( new Event::VTXEventPtr<Model::Molecule>( Event::Global::MOLECULE_CREATED, generatedMolecule ) );

			VTXApp::get().getScene().addMolecule( generatedMolecule );

			VTX::Selection::SelectionManager::get().getSelectionModel().clear();
		}

	  private:
		const Model::Residue & _target;
	};
} // namespace VTX::Action::Residue
#endif
