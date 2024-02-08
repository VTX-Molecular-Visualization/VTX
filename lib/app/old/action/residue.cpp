#include "app/old/action/residue.hpp"
#include "app/old/application/representation/representation_library.hpp"
#include "app/old/application/representation/representation_manager.hpp"
#include "app/old/application/representation/representation_preset.hpp"
#include "app/old/component/chemistry/generated_molecule.hpp"
#include "app/old/component/chemistry/molecule.hpp"
#include "app/old/component/chemistry/residue.hpp"
#include "app/old/application/selection/selection.hpp"
#include "app/old/mvc.hpp"
#include "app/old/application/scene.hpp"
#include "app/old/application/selection/selection_manager.hpp"
#include "app/old/util/molecule.hpp"
#include "app/old/vtx_app.hpp"
#include <map>
#include <unordered_set>
#include <vector>

namespace VTX::App::Old::Action::Residue
{
	void ChangeColor::execute()
	{
		std::unordered_set<App::Old::Component::Chemistry::Molecule *> molecules
			= std::unordered_set<App::Old::Component::Chemistry::Molecule *>();

		for ( App::Old::Component::Chemistry::Residue * const residue : _residues )
		{
			residue->setColor( _color );
			molecules.emplace( residue->getMolecule() );
		}

		for ( App::Old::Component::Chemistry::Molecule * const molecule : molecules )
		{
			molecule->refreshColors();
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeVisibility::execute()
	{
		if ( _mode == VISIBILITY_MODE::SOLO )
		{
			std::map<App::Old::Component::Chemistry::Molecule *, std::vector<uint>> residuesIDsPerMolecules
				= std::map<App::Old::Component::Chemistry::Molecule *, std::vector<uint>>();

			for ( Component::Generic::BaseVisible * const visible : _visibles )
			{
				App::Old::Component::Chemistry::Residue * const residue
					= static_cast<App::Old::Component::Chemistry::Residue *>( visible );
				residuesIDsPerMolecules[ residue->getMoleculePtr() ].emplace_back( residue->getIndex() );
			}

			for ( const App::Old::Application::Scene::PairMoleculePtrFloat & sceneMolecule :
				  App::Old::VTXApp::get().getScene().getMolecules() )
			{
				App::Old::Component::Chemistry::Molecule * const molecule = sceneMolecule.first;

				std::map<App::Old::Component::Chemistry::Molecule *, std::vector<uint>>::iterator it
					= residuesIDsPerMolecules.find( molecule );

				if ( it != residuesIDsPerMolecules.end() )
				{
					Util::App::Old::Molecule::soloResidues( *molecule, it->second, false );
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
			std::map<App::Old::Component::Chemistry::Molecule *, std::vector<App::Old::Component::Chemistry::Residue *>>
				residuesPerMolecules
				= std::map<App::Old::Component::Chemistry::Molecule *, std::vector<App::Old::Component::Chemistry::Residue *>>();

			for ( Component::Generic::BaseVisible * const visible : _visibles )
			{
				App::Old::Component::Chemistry::Residue * const residue
					= static_cast<App::Old::Component::Chemistry::Residue *>( visible );
				residuesPerMolecules[ residue->getMoleculePtr() ].emplace_back( residue );
			}

			for ( const std::pair<App::Old::Component::Chemistry::Molecule * const,
								  std::vector<App::Old::Component::Chemistry::Residue *>> & pair : residuesPerMolecules )
			{
				for ( App::Old::Component::Chemistry::Residue * const residue : pair.second )
					Util::App::Old::Molecule::show( *residue, _getVisibilityBool( *residue ), true, false, false );

				pair.first->notifyVisibilityChange();
				pair.first->refreshVisibilities();
				pair.first->computeRepresentationTargets();
			}
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeRepresentationPreset::execute()
	{
		App::Old::Application::Representation::RepresentationPreset * const preset
			= App::Old::Application::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

		App::Old::Application::Representation::RepresentationManager::get().instantiateRepresentations( preset, _residues );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void RemoveRepresentation::execute()
	{
		App::Old::Application::Representation::RepresentationManager::get().removeInstantiatedRepresentations( _residues );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void Delete::execute()
	{
		VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().unselectResidue( _residue );

		App::Old::Component::Chemistry::Molecule * const molecule = _residue.getMoleculePtr();
		molecule->removeResidue( _residue.getIndex() );

		if ( molecule->isEmpty() )
		{
			App::Old::VTXApp::get().getScene().removeMolecule( molecule );
			VTX::MVC_MANAGER().deleteModel( molecule );
		}
		else
		{
			molecule->refreshStructure();
			molecule->computeAllRepresentationData();
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void Copy::execute()
	{
		App::Old::Component::Chemistry::GeneratedMolecule * generatedMolecule
			= VTX::MVC_MANAGER().instantiateModel<App::Old::Component::Chemistry::GeneratedMolecule>();

		generatedMolecule->copyFromResidue( _target );
		generatedMolecule->applyTransform( _target.getMoleculePtr()->getTransform() );

		App::Old::VTXApp::get().getScene().addMolecule( generatedMolecule );
	}

	void Extract::execute()
	{
		VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().clear();

		App::Old::Component::Chemistry::GeneratedMolecule * const generatedMolecule
			= VTX::MVC_MANAGER().instantiateModel<App::Old::Component::Chemistry::GeneratedMolecule>();

		generatedMolecule->extractResidue( _target );
		App::Old::VTXApp::get().getScene().addMolecule( generatedMolecule );

		VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().selectMolecule( *generatedMolecule );
	}

	void ApplyRepresentation::execute()
	{
		App::Old::Application::Representation::RepresentationManager::get().applyRepresentation(
			_residues, _representation, _flag );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

} // namespace VTX::App::Old::Action::Residue