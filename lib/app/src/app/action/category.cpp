#include "app/action/category.hpp"
#include "app/model/category_enum.hpp"
// #include "app/model/chain.hpp"
// #include "app/model/generated_molecule.hpp"
#include "app/model/molecule.hpp"
// #include "app/model/representation/representation_library.hpp"
// #include "app/model/selection.hpp"
// #include "app/core/mvc/mvc_manager.hpp"
#include "app/old_app/object3d/scene.hpp"
// #include "app/old_app/representation/representation_manager.hpp"
// #include "app/old_app/selection/selection_manager.hpp"
#include "app/old_app/util/molecule.hpp"
#include "app/old_app/vtx_app.hpp"
#include <unordered_set>

namespace VTX::Action::Category
{
	// void ChangeColor::execute()
	//{
	//	std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

	//	for ( Model::Category * const chain : _categories )
	//	{
	//		chain->setColor( _color );
	//		molecules.emplace( chain->getMolecule() );
	//	}

	//	for ( Model::Molecule * const molecule : molecules )
	//	{
	//		molecule->refreshColors();
	//	}

	//	VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//}

	void ChangeVisibility::execute()
	{
		if ( _mode == VISIBILITY_MODE::SOLO )
		{
			std::map<Model::Molecule *, std::vector<CATEGORY_ENUM>> categoriesPerMolecules
				= std::map<Model::Molecule *, std::vector<CATEGORY_ENUM>>();

			for ( Generic::BaseVisible * const visible : _visibles )
			{
				Model::Category * const category = static_cast<Model::Category *>( visible );
				categoriesPerMolecules[ category->getMoleculePtr() ].emplace_back( category->getCategoryEnum() );
			}

			for ( const Object3D::Scene::PairMoleculePtrFloat & sceneMolecule :
				  VTXApp::get().getScene().getMolecules() )
			{
				Model::Molecule * const molecule = sceneMolecule.first;

				std::map<Model::Molecule *, std::vector<CATEGORY_ENUM>>::iterator it
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
			std::map<Model::Molecule *, std::vector<Model::Category *>> categoriesPerMolecules
				= std::map<Model::Molecule *, std::vector<Model::Category *>>();

			for ( Generic::BaseVisible * const visible : _visibles )
			{
				Model::Category * const category = static_cast<Model::Category *>( visible );
				categoriesPerMolecules[ category->getMoleculePtr() ].emplace_back( category );
			}

			for ( const std::pair<Model::Molecule * const, std::vector<Model::Category *>> & pair :
				  categoriesPerMolecules )
			{
				for ( Model::Category * const category : pair.second )
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
	//	std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

	//	for ( const Model::Category * const chain : _categories )
	//	{
	//		chain->removeChildrenRepresentations();
	//		molecules.emplace( chain->getMolecule() );
	//	}

	//	for ( Model::Molecule * const molecule : molecules )
	//	{
	//		molecule->computeAllRepresentationData();
	//	}

	//	VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//}

	// void Delete::execute()
	//{
	//	VTX::Selection::SelectionManager::get().getSelectionModel().unselectChain( _category );

	//	Model::Molecule * const molecule = _category.getMoleculePtr();
	//	molecule->removeChain( _category.getIndex() );

	//	if ( molecule->isEmpty() )
	//	{
	//		VTXApp::get().getScene().removeMolecule( molecule );
	//		VTX::Core::MVC::MvcManager::get().deleteModel( molecule );
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
	//	Model::GeneratedMolecule * generatedMolecule
	//		= VTX::Core::MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

	//	generatedMolecule->copyFromChain( _target );
	//	generatedMolecule->applyTransform( _target.getMoleculePtr()->getTransform() );

	//	VTXApp::get().getScene().addMolecule( generatedMolecule );
	//}

	// void Extract::execute()
	//{
	//	VTX::Selection::SelectionManager::get().getSelectionModel().clear();

	//	Model::GeneratedMolecule * const generatedMolecule
	//		= VTX::Core::MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

	//	generatedMolecule->extractChain( _target );
	//	VTXApp::get().getScene().addMolecule( generatedMolecule );

	//	VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecule( *generatedMolecule );
	//}

	// void ApplyRepresentation::execute()
	//{
	//	Representation::RepresentationManager::get().applyRepresentation( _categories, _representation, _flag );
	//	VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//}

} // namespace VTX::Action::Category
