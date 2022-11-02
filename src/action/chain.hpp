#ifndef __VTX_ACTION_CHAIN__
#define __VTX_ACTION_CHAIN__

#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "generic/base_colorable.hpp"
#include "model/chain.hpp"
#include "model/generated_molecule.hpp"
#include "model/molecule.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/secondary_structure.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "representation/representation_manager.hpp"
#include "selection/selection_manager.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "util/molecule.hpp"
#include "visible.hpp"
#include <unordered_set>

namespace VTX::Action::Chain
{
	class ChangeColor : public BaseAction
	{
	  public:
		explicit ChangeColor( Model::Chain & p_chain, const Color::Rgb & p_color ) :
			_color( p_color ), _chains { &p_chain }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeColor( const std::unordered_set<Model::Chain *> & p_chains, const Color::Rgb & p_color ) :
			_color( p_color ), _chains( p_chains )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

			for ( Model::Chain * const chain : _chains )
			{
				chain->setColor( _color );
				molecules.emplace( chain->getMolecule() );
			}

			for ( Model::Molecule * const molecule : molecules )
			{
				molecule->refreshColors();
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Chain *> _chains = std::unordered_set<Model::Chain *>();
		const Color::Rgb						 _color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( Model::Chain & p_chain, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_chain, p_mode )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			if ( _mode == VISIBILITY_MODE::SOLO )
			{
				std::map<Model::Molecule *, std::vector<uint>> chainsIDsPerMolecules
					= std::map<Model::Molecule *, std::vector<uint>>();

				for ( Generic::BaseVisible * const visible : _visibles )
				{
					Model::Chain * const chain = static_cast<Model::Chain *>( visible );
					chainsIDsPerMolecules[ chain->getMoleculePtr() ].emplace_back( chain->getIndex() );
				}

				for ( const Object3D::Scene::PairMoleculePtrFloat & sceneMolecule :
					  VTXApp::get().getScene().getMolecules() )
				{
					Model::Molecule * const molecule = sceneMolecule.first;

					std::map<Model::Molecule *, std::vector<uint>>::iterator it
						= chainsIDsPerMolecules.find( molecule );

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
				std::map<Model::Molecule *, std::vector<Model::Chain *>> chainsPerMolecules
					= std::map<Model::Molecule *, std::vector<Model::Chain *>>();

				for ( Generic::BaseVisible * const visible : _visibles )
				{
					Model::Chain * const chain = static_cast<Model::Chain *>( visible );
					chainsPerMolecules[ chain->getMoleculePtr() ].emplace_back( chain );
				}

				for ( const std::pair<Model::Molecule * const, std::vector<Model::Chain *>> & pair :
					  chainsPerMolecules )
				{
					for ( Model::Chain * const chain : pair.second )
						Util::Molecule::show( *chain, _getVisibilityBool( *chain ), true, false, false );

					pair.first->notifyVisibilityChange();
					pair.first->refreshVisibilities();
					pair.first->computeRepresentationTargets();
				}
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}
	};

	class ChangeRepresentationPreset : public BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( Model::Chain & p_chain, const int p_indexPreset ) :
			_indexPreset( p_indexPreset ), _chains { &p_chain }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeRepresentationPreset( const std::unordered_set<Model::Chain *> & p_chains,
											 const int									p_indexPreset ) :
			_indexPreset( p_indexPreset ),
			_chains( p_chains )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			Model::Representation::Representation * const preset
				= Model::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

			Representation::RepresentationManager::get().instantiateRepresentations( preset, _chains );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Chain *> _chains = std::unordered_set<Model::Chain *>();
		const int								 _indexPreset;
	};

	class RemoveRepresentation : public BaseAction
	{
	  public:
		explicit RemoveRepresentation( Model::Chain & p_chain ) : _chains { &p_chain }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveRepresentation( const std::unordered_set<Model::Chain *> & p_chains ) : _chains( p_chains )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			Representation::RepresentationManager::get().removeInstantiatedRepresentations( _chains );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Chain *> _chains;
	};

	class RemoveChildrenRepresentations : public BaseAction
	{
	  public:
		explicit RemoveChildrenRepresentations( Model::Chain & p_chain ) : _chains { &p_chain }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveChildrenRepresentations( const std::unordered_set<Model::Chain *> & p_chains ) :
			_chains( p_chains )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

			for ( const Model::Chain * const chain : _chains )
			{
				chain->removeChildrenRepresentations();
				molecules.emplace( chain->getMolecule() );
			}

			for ( Model::Molecule * const molecule : molecules )
			{
				molecule->computeAllRepresentationData();
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Chain *> _chains;
	};

	class Orient : public BaseAction
	{
	  public:
		explicit Orient( const Model::Chain & p_chain ) : _chain( p_chain ) {}

		virtual void execute() override
		{
			VTXApp::get()
				.getStateMachine()
				.getState<State::Visualization>( ID::State::VISUALIZATION )
				->orientCameraController( _chain.getWorldAABB() );
		}

	  private:
		const Model::Chain & _chain;
	};

	class Delete : public BaseAction
	{
	  public:
		explicit Delete( Model::Chain & p_chain ) : _chain( p_chain )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			VTX::Selection::SelectionManager::get().getSelectionModel().unselectChain( _chain );

			Model::Molecule * const molecule = _chain.getMoleculePtr();
			molecule->removeChain( _chain.getIndex() );

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

	  private:
		Model::Chain & _chain;
	};

	class Copy : public BaseAction
	{
	  public:
		explicit Copy( const Model::Chain & p_target ) : _target( p_target )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override
		{
			Model::GeneratedMolecule * generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->copyFromChain( _target );
			generatedMolecule->applyTransform( _target.getMoleculePtr()->getTransform() );

			VTXApp::get().getScene().addMolecule( generatedMolecule );
		}

	  private:
		const Model::Chain & _target;
	};

	class Extract : public BaseAction
	{
	  public:
		explicit Extract( const Model::Chain & p_target ) : _target( p_target )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override
		{
			VTX::Selection::SelectionManager::get().getSelectionModel().clear();

			Model::GeneratedMolecule * const generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->extractChain( _target );
			VTXApp::get().getScene().addMolecule( generatedMolecule );

			VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecule( *generatedMolecule );
		}

	  private:
		const Model::Chain & _target;
	};

	class ApplyRepresentation : public BaseAction
	{
	  public:
		explicit ApplyRepresentation( const std::unordered_set<Model::Chain *> &				p_chains,
									  const Model::Representation::InstantiatedRepresentation & p_source,
									  const Model::Representation::MEMBER_FLAG &				p_flag ) :
			_representation( p_source ),
			_flag( p_flag ), _chains( p_chains )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			Representation::RepresentationManager::get().applyRepresentation( _chains, _representation, _flag );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Chain *>				  _chains = std::unordered_set<Model::Chain *>();
		const Model::Representation::InstantiatedRepresentation & _representation;
		const Model::Representation::MEMBER_FLAG				  _flag;
	};

} // namespace VTX::Action::Chain
#endif
