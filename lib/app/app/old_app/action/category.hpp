#ifndef __VTX_ACTION_CATEGORY__
#define __VTX_ACTION_CATEGORY__

#include "old_app/event/event.hpp"
#include "old_app/event/event_manager.hpp"
#include "old_app/generic/base_colorable.hpp"
#include "old_app/model/category.hpp"
#include "old_app/model/category_enum.hpp"
#include "old_app/model/chain.hpp"
#include "old_app/model/generated_molecule.hpp"
#include "old_app/model/molecule.hpp"
#include "old_app/model/representation/instantiated_representation.hpp"
#include "old_app/model/representation/representation_library.hpp"
#include "old_app/model/secondary_structure.hpp"
#include "old_app/model/selection.hpp"
#include "old_app/mvc/mvc_manager.hpp"
#include "old_app/object3d/scene.hpp"
#include "old_app/representation/representation_manager.hpp"
#include "old_app/selection/selection_manager.hpp"
#include "old_app/util/molecule.hpp"
#include "visible.hpp"
#include <unordered_set>

namespace VTX::Action::Category
{
	// class ChangeColor : public BaseAction
	//{
	//  public:
	//	explicit ChangeColor( Model::Category & p_category, const Color::Rgb & p_color ) :
	//		_color( p_color ), _categories { &p_category }
	//	{
	//		_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit ChangeColor( const std::unordered_set<Model::Category *> & p_categories, const Color::Rgb & p_color ) :
	//		_color( p_color ), _categories( p_categories )
	//	{
	//		_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override
	//	{
	//		std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

	//		for ( Model::Category * const chain : _categories )
	//		{
	//			chain->setColor( _color );
	//			molecules.emplace( chain->getMolecule() );
	//		}

	//		for ( Model::Molecule * const molecule : molecules )
	//		{
	//			molecule->refreshColors();
	//		}

	//		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//	}

	//  private:
	//	const std::unordered_set<Model::Category *> _categories = std::unordered_set<Model::Category *>();
	//	const Color::Rgb						 _color;
	//};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( Model::Category & p_category, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_category, p_mode )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
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
	};

	// class ChangeRepresentationPreset : public BaseAction
	//{
	//  public:
	//	explicit ChangeRepresentationPreset( Model::Category & p_category, const int p_indexPreset ) :
	//		_indexPreset( p_indexPreset ), _categories { &p_category }
	//	{
	//		_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit ChangeRepresentationPreset( const std::unordered_set<Model::Category *> & p_categories,
	//										 const int									p_indexPreset ) :
	//		_indexPreset( p_indexPreset ),
	//		_categories( p_categories )
	//	{
	//		_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override
	//	{
	//		Model::Representation::Representation * const preset
	//			= Model::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

	//		Representation::RepresentationManager::get().instantiateRepresentations( preset, _categories );
	//		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//	}

	//  private:
	//	const std::unordered_set<Model::Category *> _categories = std::unordered_set<Model::Category *>();
	//	const int								 _indexPreset;
	//};

	// class RemoveRepresentation : public BaseAction
	//{
	//  public:
	//	explicit RemoveRepresentation( Model::Category & p_category ) : _categories { &p_category }
	//	{
	//		_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit RemoveRepresentation( const std::unordered_set<Model::Category *> & p_categories ) : _categories(
	// p_categories )
	//	{
	//		_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override
	//	{
	//		Representation::RepresentationManager::get().removeInstantiatedRepresentations( _categories );
	//		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//	}

	//  private:
	//	const std::unordered_set<Model::Category *> _categories;
	//};

	// class RemoveChildrenRepresentations : public BaseAction
	//{
	//  public:
	//	explicit RemoveChildrenRepresentations( Model::Category & p_category ) : _categories { &p_category }
	//	{
	//		_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
	//	}
	//	explicit RemoveChildrenRepresentations( const std::unordered_set<Model::Category *> & p_categories ) :
	//		_categories( p_categories )
	//	{
	//		_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override
	//	{
	//		std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

	//		for ( const Model::Category * const chain : _categories )
	//		{
	//			chain->removeChildrenRepresentations();
	//			molecules.emplace( chain->getMolecule() );
	//		}

	//		for ( Model::Molecule * const molecule : molecules )
	//		{
	//			molecule->computeAllRepresentationData();
	//		}

	//		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//	}

	//  private:
	//	const std::unordered_set<Model::Category *> _categories;
	//};

	// class Delete : public BaseAction
	//{
	//  public:
	//	explicit Delete( Model::Category & p_category ) : _category( p_category )
	//	{
	//		_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override
	//	{
	//		VTX::Selection::SelectionManager::get().getSelectionModel().unselectChain( _category );

	//		Model::Molecule * const molecule = _category.getMoleculePtr();
	//		molecule->removeChain( _category.getIndex() );

	//		if ( molecule->isEmpty() )
	//		{
	//			VTXApp::get().getScene().removeMolecule( molecule );
	//			MVC::MvcManager::get().deleteModel( molecule );
	//		}
	//		else
	//		{
	//			molecule->refreshStructure();
	//			molecule->computeAllRepresentationData();
	//		}

	//		VTXApp::get().MASK |= VTX_MASK_SELECTION_UPDATED;
	//		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//	}

	//  private:
	//	Model::Category & _category;
	//};

	// class Copy : public BaseAction
	//{
	//  public:
	//	explicit Copy( const Model::Category & p_target ) : _target( p_target )
	//	{
	//		_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
	//	}
	//	virtual void execute() override
	//	{
	//		Model::GeneratedMolecule * generatedMolecule
	//			= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

	//		generatedMolecule->copyFromChain( _target );
	//		generatedMolecule->applyTransform( _target.getMoleculePtr()->getTransform() );

	//		VTXApp::get().getScene().addMolecule( generatedMolecule );
	//	}

	//  private:
	//	const Model::Category & _target;
	//};

	// class Extract : public BaseAction
	//{
	//  public:
	//	explicit Extract( const Model::Category & p_target ) : _target( p_target )
	//	{
	//		_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
	//	}
	//	virtual void execute() override
	//	{
	//		VTX::Selection::SelectionManager::get().getSelectionModel().clear();

	//		Model::GeneratedMolecule * const generatedMolecule
	//			= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

	//		generatedMolecule->extractChain( _target );
	//		VTXApp::get().getScene().addMolecule( generatedMolecule );

	//		VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecule( *generatedMolecule );
	//	}

	//  private:
	//	const Model::Category & _target;
	//};

	// class ApplyRepresentation : public BaseAction
	//{
	//  public:
	//	explicit ApplyRepresentation( const std::unordered_set<Model::Category *> &				p_categories,
	//								  const Model::Representation::InstantiatedRepresentation & p_source,
	//								  const Model::Representation::MEMBER_FLAG &				p_flag ) :
	//		_representation( p_source ),
	//		_flag( p_flag ), _categories( p_categories )
	//	{
	//		_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
	//	}

	//	virtual void execute() override
	//	{
	//		Representation::RepresentationManager::get().applyRepresentation( _categories, _representation, _flag );
	//		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	//	}

	//  private:
	//	const std::unordered_set<Model::Category *>				  _categories = std::unordered_set<Model::Category *>();
	//	const Model::Representation::InstantiatedRepresentation & _representation;
	//	const Model::Representation::MEMBER_FLAG				  _flag;
	//};

} // namespace VTX::Action::Category
#endif
