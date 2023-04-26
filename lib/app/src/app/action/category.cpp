#include "app/action/category.hpp"
#include "app/internal/chemdb/category.hpp"
// #include "app/component/chemistry/chain.hpp"
// #include "app/component/chemistry/generated_molecule.hpp"
#include "app/component/chemistry/molecule.hpp"
// #include "app/model/representation/representation_library.hpp"
// #include "app/model/selection.hpp"
// #include "app/mvc.hpp"
#include "app/old_app/object3d/scene.hpp"
// #include "app/old_app/representation/representation_manager.hpp"
// #include "app/old_app/selection/selection_manager.hpp"
#include "app/old_app/util/molecule.hpp"
#include "app/old_app/vtx_app.hpp"
#include <unordered_set>

namespace VTX::App::Action::Category
{
	// void ChangeColor::execute()
	//{
	//	std::unordered_set<App::Component::Chemistry::Molecule *> molecules =
	// std::unordered_set<App::Component::Chemistry::Molecule *>();

	//	for ( App::Component::Chemistry::Category * const chain : _categories )
	//	{
	//		chain->setColor( _color );
	//		molecules.emplace( chain->getMolecule() );
	//	}

	//	for ( App::Component::Chemistry::Molecule * const molecule : molecules )
	//	{
	//		molecule->refreshColors();
	//	}

	//	VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//}

	void ChangeVisibility::execute()
	{
		if ( _mode == VISIBILITY_MODE::SOLO )
		{
			std::map<App::Component::Chemistry::Molecule *, std::vector<App::Internal::ChemDB::Category::TYPE>>
				categoriesPerMolecules
				= std::map<App::Component::Chemistry::Molecule *, std::vector<App::Internal::ChemDB::Category::TYPE>>();

			for ( Generic::BaseVisible * const visible : _visibles )
			{
				App::Component::Chemistry::Category * const category
					= static_cast<App::Component::Chemistry::Category *>( visible );
				categoriesPerMolecules[ category->getMoleculePtr() ].emplace_back( category->getCategoryEnum() );
			}

			for ( const Object3D::Scene::PairMoleculePtrFloat & sceneMolecule :
				  VTXApp::get().getScene().getMolecules() )
			{
				App::Component::Chemistry::Molecule * const molecule = sceneMolecule.first;

				std::map<App::Component::Chemistry::Molecule *,
						 std::vector<App::Internal::ChemDB::Category::TYPE>>::iterator it
					= categoriesPerMolecules.find( molecule );

				if ( it != categoriesPerMolecules.end() )
				{
					Util::Molecule::soloCategories( *molecule, it->second, false );
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
			std::map<App::Component::Chemistry::Molecule *, std::vector<App::Component::Chemistry::Category *>>
				categoriesPerMolecules
				= std::map<App::Component::Chemistry::Molecule *, std::vector<App::Component::Chemistry::Category *>>();

			for ( Generic::BaseVisible * const visible : _visibles )
			{
				App::Component::Chemistry::Category * const category
					= static_cast<App::Component::Chemistry::Category *>( visible );
				categoriesPerMolecules[ category->getMoleculePtr() ].emplace_back( category );
			}

			for ( const std::pair<App::Component::Chemistry::Molecule * const,
								  std::vector<App::Component::Chemistry::Category *>> & pair : categoriesPerMolecules )
			{
				for ( App::Component::Chemistry::Category * const category : pair.second )
					Util::Molecule::show( *category, _getVisibilityBool( *category ), true, false, false );

				pair.first->notifyVisibilityChange();
				pair.first->refreshVisibilities();
				pair.first->computeRepresentationTargets();
			}
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	// void ChangeRepresentationPreset::execute()
	//{
	//	Model::Representation::Representation * const preset
	//		= Model::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

	//	Representation::RepresentationManager::get().instantiateRepresentations( preset, _categories );
	//	VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//}

	// void RemoveRepresentation::execute()
	//{
	//	Representation::RepresentationManager::get().removeInstantiatedRepresentations( _categories );
	//	VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	// }

	// void RemoveChildrenRepresentations::execute()
	//{
	//	std::unordered_set<App::Component::Chemistry::Molecule *> molecules =
	// std::unordered_set<App::Component::Chemistry::Molecule *>();

	//	for ( const App::Component::Chemistry::Category * const chain : _categories )
	//	{
	//		chain->removeChildrenRepresentations();
	//		molecules.emplace( chain->getMolecule() );
	//	}

	//	for ( App::Component::Chemistry::Molecule * const molecule : molecules )
	//	{
	//		molecule->computeAllRepresentationData();
	//	}

	//	VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//}

	// void Delete::execute()
	//{
	//	VTX::Selection::SelectionManager::get().getSelectionModel().unselectChain( _category );

	//	App::Component::Chemistry::Molecule * const molecule = _category.getMoleculePtr();
	//	molecule->removeChain( _category.getIndex() );

	//	if ( molecule->isEmpty() )
	//	{
	//		VTXApp::get().getScene().removeMolecule( molecule );
	//		VTX::MVC_MANAGER().deleteModel( molecule );
	//	}
	//	else
	//	{
	//		molecule->refreshStructure();
	//		molecule->computeAllRepresentationData();
	//	}

	//	VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	//	VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//}

	// void Copy::execute()
	//{
	//	App::Component::Chemistry::GeneratedMolecule * generatedMolecule
	//		= VTX::MVC_MANAGER().instantiateModel<App::Component::Chemistry::GeneratedMolecule>();

	//	generatedMolecule->copyFromChain( _target );
	//	generatedMolecule->applyTransform( _target.getMoleculePtr()->getTransform() );

	//	VTXApp::get().getScene().addMolecule( generatedMolecule );
	//}

	// void Extract::execute()
	//{
	//	VTX::Selection::SelectionManager::get().getSelectionModel().clear();

	//	App::Component::Chemistry::GeneratedMolecule * const generatedMolecule
	//		= VTX::MVC_MANAGER().instantiateModel<App::Component::Chemistry::GeneratedMolecule>();

	//	generatedMolecule->extractChain( _target );
	//	VTXApp::get().getScene().addMolecule( generatedMolecule );

	//	VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecule( *generatedMolecule );
	//}

	// void ApplyRepresentation::execute()
	//{
	//	Representation::RepresentationManager::get().applyRepresentation( _categories, _representation, _flag );
	//	VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//}

} // namespace VTX::App::Action::Category
