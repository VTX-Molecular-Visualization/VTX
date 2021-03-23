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
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "util/molecule.hpp"
#include "visible.hpp"
#include <unordered_set>

namespace VTX::Action::Residue
{
	class ChangeColor : public BaseAction
	{
	  public:
		explicit ChangeColor( Model::Residue & p_residue, const Color::Rgb & p_color ) : _color( p_color )
		{
			_residues.emplace( &p_residue );
		}
		explicit ChangeColor( const std::unordered_set<Model::Residue *> & p_residues, const Color::Rgb & p_color ) :
			_color( p_color )
		{
			for ( Model::Residue * const residue : p_residues )
				_residues.emplace( residue );
		}

		virtual void execute() override
		{
			std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

			for ( Model::Residue * const residue : _residues )
			{
				residue->setColor( _color );
				molecules.emplace( residue->getMolecule() );
			}

			for ( Model::Molecule * const molecule : molecules )
			{
				molecule->refreshColors();
				molecule->getSecondaryStructure().refreshColors();
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Model::Residue *> _residues = std::unordered_set<Model::Residue *>();
		const Color::Rgb					 _color;
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

	class ChangeRepresentationPreset : public BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( Model::Residue & p_residue, const int p_indexPreset ) :
			_indexPreset( p_indexPreset )
		{
			_residues.emplace( &p_residue );
		}
		explicit ChangeRepresentationPreset( const std::unordered_set<Model::Residue *> & p_residues,
											 const int									  p_indexPreset ) :
			_indexPreset( p_indexPreset )
		{
			for ( Model::Residue * const residue : p_residues )
				_residues.emplace( residue );
		}

		virtual void execute() override
		{
			Model::Representation::BaseRepresentation * const preset
				= Model::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

			std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

			for ( Model::Residue * const residue : _residues )
			{
				Model::Representation::InstantiatedRepresentation * const instantiatedRepresentation
					= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>(
						preset );

				residue->applyRepresentation( instantiatedRepresentation, false );

				molecules.emplace( residue->getMolecule() );
			}

			for ( Model::Molecule * const molecule : molecules )
				molecule->computeAllRepresentationData();

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Model::Residue *> _residues = std::unordered_set<Model::Residue *>();
		const int							 _indexPreset;
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

	class ApplyRepresentation : public BaseAction
	{
	  public:
		explicit ApplyRepresentation( const std::unordered_set<Model::Residue *> &				p_residues,
									  const Model::Representation::InstantiatedRepresentation & p_source,
									  const Model::Representation::MEMBER_FLAG &				p_flag ) :
			_representation( p_source ),
			_flag( p_flag )
		{
			for ( Model::Residue * const residue : p_residues )
				_residues.emplace( residue );
		}

		virtual void execute() override
		{
			std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

			for ( Model::Residue * const residue : _residues )
			{
				Model::Representation::InstantiatedRepresentation * residueRepresentation;

				if ( residue->hasCustomRepresentation() )
				{
					residueRepresentation = residue->getCustomRepresentation();
				}
				else
				{
					residueRepresentation
						= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>(
							residue->getRepresentation() );

					// Compute molecules at end
					residue->applyRepresentation( residueRepresentation, false );
				}

				residueRepresentation->applyData( _representation, _flag, false );
				molecules.emplace( residue->getMolecule() );
			}

			for ( Model::Molecule * const molecule : molecules )
				molecule->computeAllRepresentationData();

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Model::Residue *>					  _residues = std::unordered_set<Model::Residue *>();
		const Model::Representation::InstantiatedRepresentation & _representation;
		const Model::Representation::MEMBER_FLAG				  _flag;
	};
} // namespace VTX::Action::Residue
#endif
