#include "app/action/chain.hpp"
#include "app/mvc.hpp"
#include "app/component/chemistry/generated_molecule.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/model/representation/representation.hpp"
#include "app/model/representation/representation_library.hpp"
#include "app/model/selection.hpp"
#include "app/old_app/generic/base_visible.hpp"
#include "app/old_app/object3d/scene.hpp"
#include "app/old_app/representation/representation_manager.hpp"
#include "app/old_app/selection/selection_manager.hpp"
#include "app/old_app/util/molecule.hpp"
#include "app/old_app/vtx_app.hpp"
#include <map>
#include <vector>

namespace VTX::App::Action::Chain
{
	void ChangeColor::execute()
	{
		std::unordered_set<App::Component::Chemistry::Molecule *> molecules = std::unordered_set<App::Component::Chemistry::Molecule *>();

		for ( App::Component::Chemistry::Chain * const chain : _chains )
		{
			chain->setColor( _color );
			molecules.emplace( chain->getMolecule() );
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
			std::map<App::Component::Chemistry::Molecule *, std::vector<uint>> chainsIDsPerMolecules
				= std::map<App::Component::Chemistry::Molecule *, std::vector<uint>>();

			for ( Generic::BaseVisible * const visible : _visibles )
			{
				App::Component::Chemistry::Chain * const chain = static_cast<App::Component::Chemistry::Chain *>( visible );
				chainsIDsPerMolecules[ chain->getMoleculePtr() ].emplace_back( chain->getIndex() );
			}

			for ( const Object3D::Scene::PairMoleculePtrFloat & sceneMolecule :
				  VTXApp::get().getScene().getMolecules() )
			{
				App::Component::Chemistry::Molecule * const molecule = sceneMolecule.first;

				std::map<App::Component::Chemistry::Molecule *, std::vector<uint>>::iterator it = chainsIDsPerMolecules.find( molecule );

				if ( it != chainsIDsPerMolecules.end() )
				{
					Util::Molecule::soloChains( *molecule, it->second, false );
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
			std::map<App::Component::Chemistry::Molecule *, std::vector<App::Component::Chemistry::Chain *>> chainsPerMolecules
				= std::map<App::Component::Chemistry::Molecule *, std::vector<App::Component::Chemistry::Chain *>>();

			for ( Generic::BaseVisible * const visible : _visibles )
			{
				App::Component::Chemistry::Chain * const chain = static_cast<App::Component::Chemistry::Chain *>( visible );
				chainsPerMolecules[ chain->getMoleculePtr() ].emplace_back( chain );
			}

			for ( const std::pair<App::Component::Chemistry::Molecule * const, std::vector<App::Component::Chemistry::Chain *>> & pair : chainsPerMolecules )
			{
				for ( App::Component::Chemistry::Chain * const chain : pair.second )
					Util::Molecule::show( *chain, _getVisibilityBool( *chain ), true, false, false );

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

		Representation::RepresentationManager::get().instantiateRepresentations( preset, _chains );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void RemoveRepresentation::execute()
	{
		Representation::RepresentationManager::get().removeInstantiatedRepresentations( _chains );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void RemoveChildrenRepresentations::execute()
	{
		std::unordered_set<App::Component::Chemistry::Molecule *> molecules = std::unordered_set<App::Component::Chemistry::Molecule *>();

		for ( App::Component::Chemistry::Chain * const chain : _chains )
		{
			chain->removeChildrenRepresentations();
			molecules.emplace( chain->getMolecule() );
		}

		for ( App::Component::Chemistry::Molecule * const molecule : molecules )
		{
			molecule->computeAllRepresentationData();
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void Delete::execute()
	{
		VTX::Selection::SelectionManager::get().getSelectionModel().unselectChain( _chain );

		App::Component::Chemistry::Molecule * const molecule = _chain.getMoleculePtr();
		molecule->removeChain( _chain.getIndex() );

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

		generatedMolecule->copyFromChain( _target );
		generatedMolecule->applyTransform( _target.getMoleculePtr()->getTransform() );

		VTXApp::get().getScene().addMolecule( generatedMolecule );
	}

	void Extract::execute()
	{
		VTX::Selection::SelectionManager::get().getSelectionModel().clear();

		App::Component::Chemistry::GeneratedMolecule * const generatedMolecule
			= VTX::MVC_MANAGER().instantiateModel<App::Component::Chemistry::GeneratedMolecule>();

		generatedMolecule->extractChain( _target );
		VTXApp::get().getScene().addMolecule( generatedMolecule );

		VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecule( *generatedMolecule );
	}

	void ApplyRepresentation::execute()
	{
		Representation::RepresentationManager::get().applyRepresentation( _chains, _representation, _flag );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

} // namespace VTX::App::Action::Chain
