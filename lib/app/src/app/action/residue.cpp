#include "app/action/residue.hpp"
#include "app/application/representation/representation_library.hpp"
#include "app/application/representation/representation_manager.hpp"
#include "app/application/representation/representation_preset.hpp"
#include "app/component/chemistry/generated_molecule.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/model/selection.hpp"
#include "app/mvc.hpp"
#include "app/old_app/object3d/scene.hpp"
#include "app/old_app/selection/selection_manager.hpp"
#include "app/old_app/util/molecule.hpp"
#include "app/old_app/vtx_app.hpp"
#include <map>
#include <unordered_set>
#include <vector>

namespace VTX::App::Action::Residue
{
	void ChangeColor::execute()
	{
		std::unordered_set<App::Component::Chemistry::Molecule *> molecules
			= std::unordered_set<App::Component::Chemistry::Molecule *>();

		for ( App::Component::Chemistry::Residue * const residue : _residues )
		{
			residue->setColor( _color );
			molecules.emplace( residue->getMolecule() );
		}

		for ( App::Component::Chemistry::Molecule * const molecule : molecules )
		{
			molecule->refreshColors();
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeVisibility::execute()
	{
		if ( _mode == VISIBILITY_MODE::SOLO )
		{
			std::map<App::Component::Chemistry::Molecule *, std::vector<uint>> residuesIDsPerMolecules
				= std::map<App::Component::Chemistry::Molecule *, std::vector<uint>>();

			for ( Generic::BaseVisible * const visible : _visibles )
			{
				App::Component::Chemistry::Residue * const residue
					= static_cast<App::Component::Chemistry::Residue *>( visible );
				residuesIDsPerMolecules[ residue->getMoleculePtr() ].emplace_back( residue->getIndex() );
			}

			for ( const Object3D::Scene::PairMoleculePtrFloat & sceneMolecule :
				  VTXApp::get().getScene().getMolecules() )
			{
				App::Component::Chemistry::Molecule * const molecule = sceneMolecule.first;

				std::map<App::Component::Chemistry::Molecule *, std::vector<uint>>::iterator it
					= residuesIDsPerMolecules.find( molecule );

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
			std::map<App::Component::Chemistry::Molecule *, std::vector<App::Component::Chemistry::Residue *>>
				residuesPerMolecules
				= std::map<App::Component::Chemistry::Molecule *, std::vector<App::Component::Chemistry::Residue *>>();

			for ( Generic::BaseVisible * const visible : _visibles )
			{
				App::Component::Chemistry::Residue * const residue
					= static_cast<App::Component::Chemistry::Residue *>( visible );
				residuesPerMolecules[ residue->getMoleculePtr() ].emplace_back( residue );
			}

			for ( const std::pair<App::Component::Chemistry::Molecule * const,
								  std::vector<App::Component::Chemistry::Residue *>> & pair : residuesPerMolecules )
			{
				for ( App::Component::Chemistry::Residue * const residue : pair.second )
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
		App::Application::Representation::RepresentationPreset * const preset
			= App::Application::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

		App::Application::Representation::RepresentationManager::get().instantiateRepresentations( preset, _residues );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void RemoveRepresentation::execute()
	{
		App::Application::Representation::RepresentationManager::get().removeInstantiatedRepresentations( _residues );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void Delete::execute()
	{
		VTX::Selection::SelectionManager::get().getSelectionModel().unselectResidue( _residue );

		App::Component::Chemistry::Molecule * const molecule = _residue.getMoleculePtr();
		molecule->removeResidue( _residue.getIndex() );

		if ( molecule->isEmpty() )
		{
			VTXApp::get().getScene().removeMolecule( molecule );
			VTX::MVC_MANAGER().deleteModel( molecule );
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
		App::Component::Chemistry::GeneratedMolecule * generatedMolecule
			= VTX::MVC_MANAGER().instantiateModel<App::Component::Chemistry::GeneratedMolecule>();

		generatedMolecule->copyFromResidue( _target );
		generatedMolecule->applyTransform( _target.getMoleculePtr()->getTransform() );

		VTXApp::get().getScene().addMolecule( generatedMolecule );
	}

	void Extract::execute()
	{
		VTX::Selection::SelectionManager::get().getSelectionModel().clear();

		App::Component::Chemistry::GeneratedMolecule * const generatedMolecule
			= VTX::MVC_MANAGER().instantiateModel<App::Component::Chemistry::GeneratedMolecule>();

		generatedMolecule->extractResidue( _target );
		VTXApp::get().getScene().addMolecule( generatedMolecule );

		VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecule( *generatedMolecule );
	}

	void ApplyRepresentation::execute()
	{
		App::Application::Representation::RepresentationManager::get().applyRepresentation(
			_residues, _representation, _flag );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

} // namespace VTX::App::Action::Residue
