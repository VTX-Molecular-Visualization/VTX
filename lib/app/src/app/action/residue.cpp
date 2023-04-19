#include "app/action/residue.hpp"
#include "app/old_app/model/generated_molecule.hpp"
#include "app/old_app/model/molecule.hpp"
#include "app/old_app/model/representation/representation.hpp"
#include "app/old_app/model/representation/representation_library.hpp"
#include "app/old_app/model/residue.hpp"
#include "app/old_app/model/selection.hpp"
#include "app/old_app/mvc/mvc_manager.hpp"
#include "app/old_app/object3d/scene.hpp"
#include "app/old_app/representation/representation_manager.hpp"
#include "app/old_app/selection/selection_manager.hpp"
#include "app/old_app/util/molecule.hpp"
#include "app/old_app/vtx_app.hpp"
#include <map>
#include <unordered_set>
#include <vector>

namespace VTX::Action::Residue
{
	void ChangeColor::execute()
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
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeVisibility::execute()
	{
		if ( _mode == VISIBILITY_MODE::SOLO )
		{
			std::map<Model::Molecule *, std::vector<uint>> residuesIDsPerMolecules
				= std::map<Model::Molecule *, std::vector<uint>>();

			for ( Generic::BaseVisible * const visible : _visibles )
			{
				Model::Residue * const residue = static_cast<Model::Residue *>( visible );
				residuesIDsPerMolecules[ residue->getMoleculePtr() ].emplace_back( residue->getIndex() );
			}

			for ( const Object3D::Scene::PairMoleculePtrFloat & sceneMolecule :
				  VTXApp::get().getScene().getMolecules() )
			{
				Model::Molecule * const molecule = sceneMolecule.first;

				std::map<Model::Molecule *, std::vector<uint>>::iterator it = residuesIDsPerMolecules.find( molecule );

				if ( it != residuesIDsPerMolecules.end() )
				{
					Util::Molecule::soloResidues( *molecule, it->second, false );
				}
				else
				{
					molecule->setVisible( false );
				}

				molecule->refreshVisibilities();
				molecule->computeRepresentationTargets();
			}
		}
		else
		{
			std::map<Model::Molecule *, std::vector<Model::Residue *>> residuesPerMolecules
				= std::map<Model::Molecule *, std::vector<Model::Residue *>>();

			for ( Generic::BaseVisible * const visible : _visibles )
			{
				Model::Residue * const residue = static_cast<Model::Residue *>( visible );
				residuesPerMolecules[ residue->getMoleculePtr() ].emplace_back( residue );
			}

			for ( const std::pair<Model::Molecule * const, std::vector<Model::Residue *>> & pair :
				  residuesPerMolecules )
			{
				for ( Model::Residue * const residue : pair.second )
					Util::Molecule::show( *residue, _getVisibilityBool( *residue ), true, false, false );

				pair.first->notifyVisibilityChange();
				pair.first->refreshVisibilities();
				pair.first->computeRepresentationTargets();
			}
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeRepresentationPreset::execute()
	{
		Model::Representation::Representation * const preset
			= Model::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

		Representation::RepresentationManager::get().instantiateRepresentations( preset, _residues );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void RemoveRepresentation::execute()
	{
		Representation::RepresentationManager::get().removeInstantiatedRepresentations( _residues );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void Delete::execute()
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
			molecule->refreshStructure();
			molecule->computeAllRepresentationData();
		}

		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void Copy::execute()
	{
		Model::GeneratedMolecule * generatedMolecule
			= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

		generatedMolecule->copyFromResidue( _target );
		generatedMolecule->applyTransform( _target.getMoleculePtr()->getTransform() );

		VTXApp::get().getScene().addMolecule( generatedMolecule );
	}

	void Extract::execute()
	{
		VTX::Selection::SelectionManager::get().getSelectionModel().clear();

		Model::GeneratedMolecule * const generatedMolecule
			= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

		generatedMolecule->extractResidue( _target );
		VTXApp::get().getScene().addMolecule( generatedMolecule );

		VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecule( *generatedMolecule );
	}

	void ApplyRepresentation::execute()
	{
		Representation::RepresentationManager::get().applyRepresentation( _residues, _representation, _flag );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

} // namespace VTX::Action::Residue
