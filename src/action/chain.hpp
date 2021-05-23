#ifndef __VTX_ACTION_CHAIN__
#define __VTX_ACTION_CHAIN__

#ifdef _MSC_VER
#pragma once
#endif

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
		explicit ChangeColor( Model::Chain & p_chain, const Color::Rgb & p_color ) : _color( p_color )
		{
			_chains.emplace( &p_chain );
		}
		explicit ChangeColor( const std::unordered_set<Model::Chain *> & p_chains, const Color::Rgb & p_color ) :
			_color( p_color )
		{
			for ( Model::Chain * const chain : p_chains )
				_chains.emplace( chain );
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
				molecule->getSecondaryStructure().refreshColors();
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Model::Chain *> _chains = std::unordered_set<Model::Chain *>();
		const Color::Rgb				   _color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( Model::Chain & p_chain, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_chain, p_mode )
		{
		}

		virtual void execute() override
		{
			for ( Generic::BaseVisible * const visible : _visibles )
			{
				bool		   newVisibility = _getVisibilityBool( *visible );
				Model::Chain & chain		 = *( (Model::Chain *)visible );

				chain.setVisible( newVisibility );

				if ( _mode == VISIBILITY_MODE::ALL || _mode == VISIBILITY_MODE::SOLO )
				{
					for ( Model::Chain * const c : chain.getMoleculePtr()->getChains() )
					{
						if ( c == nullptr )
							continue;

						c->setVisible( _mode == VISIBILITY_MODE::ALL
									   || ( _mode == VISIBILITY_MODE::SOLO && c == &chain ) );
					}
				}

				chain.getMoleculePtr()->refreshVisibilities();
				chain.getMoleculePtr()->computeRepresentationTargets();
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}
	};

	class ChangeRepresentationPreset : public BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( Model::Chain & p_chain, const int p_indexPreset ) :
			_indexPreset( p_indexPreset )
		{
			_chains.emplace( &p_chain );
		}
		explicit ChangeRepresentationPreset( const std::unordered_set<Model::Chain *> & p_chains,
											 const int									p_indexPreset ) :
			_indexPreset( p_indexPreset )
		{
			for ( Model::Chain * const chain : p_chains )
				_chains.emplace( chain );
		}

		virtual void execute() override
		{
			Model::Representation::Representation * const preset
				= Model::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

			Representation::RepresentationManager::get().instantiateRepresentations( preset, _chains );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Model::Chain *> _chains = std::unordered_set<Model::Chain *>();
		const int						   _indexPreset;
	};

	class RemoveRepresentation : public BaseAction
	{
	  public:
		explicit RemoveRepresentation( Model::Chain & p_chain ) { _chains.emplace( &p_chain ); }
		explicit RemoveRepresentation( const std::unordered_set<Model::Chain *> & p_chains )
		{
			for ( Model::Chain * const chain : p_chains )
				_chains.emplace( chain );
		}

		virtual void execute() override
		{
			Representation::RepresentationManager::get().removeInstantiatedRepresentations( _chains );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Model::Chain *> _chains = std::unordered_set<Model::Chain *>();
	};

	class RemoveChildrenRepresentations : public BaseAction
	{
	  public:
		explicit RemoveChildrenRepresentations( Model::Chain & p_chain ) { _chains.emplace( &p_chain ); }
		explicit RemoveChildrenRepresentations( const std::unordered_set<Model::Chain *> & p_chains )
		{
			for ( Model::Chain * const chain : p_chains )
				_chains.emplace( chain );
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
		std::unordered_set<Model::Chain *> _chains = std::unordered_set<Model::Chain *>();
	};

	class Orient : public BaseAction
	{
	  public:
		explicit Orient( Model::Chain & p_chain ) : _chain( p_chain ) {}

		virtual void execute() override
		{
			VTXApp::get()
				.getStateMachine()
				.getItem<State::Visualization>( ID::State::VISUALIZATION )
				->orientCameraController( _chain.getWorldAABB() );
		}

	  private:
		Model::Chain & _chain;
	};

	class Delete : public BaseAction
	{
	  public:
		explicit Delete( Model::Chain & p_chain ) : _chain( p_chain ) {}

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
				molecule->refreshBondsBuffer();
				molecule->refreshSecondaryStructure();
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
		explicit Copy( const Model::Chain & p_target ) : _target( p_target ) {}
		virtual void execute() override
		{
			Model::GeneratedMolecule * generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->copyFromChain( _target );

			VTX_EVENT( new Event::VTXEventPtr<Model::Molecule>( Event::Global::MOLECULE_CREATED, generatedMolecule ) );

			const float offset = generatedMolecule->getAABB().radius() + _target.getAABB().radius()
								 + VTX::Setting::COPIED_MOLECULE_OFFSET;
			generatedMolecule->setTranslation( VTX::Vec3f( offset, 0, 0 ) );

			VTXApp::get().getScene().addMolecule( generatedMolecule );
		}

	  private:
		const Model::Chain & _target;
	};

	class Extract : public BaseAction
	{
	  public:
		explicit Extract( const Model::Chain & p_target ) : _target( p_target ) {}
		virtual void execute() override
		{
			Model::GeneratedMolecule * const generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->extractChain( _target );

			VTX_EVENT( new Event::VTXEventPtr<Model::Molecule>( Event::Global::MOLECULE_CREATED, generatedMolecule ) );

			VTXApp::get().getScene().addMolecule( generatedMolecule );

			VTX::Selection::SelectionManager::get().getSelectionModel().clear();
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
			_flag( p_flag )
		{
			for ( Model::Chain * const chain : p_chains )
				_chains.emplace( chain );
		}

		virtual void execute() override
		{
			Representation::RepresentationManager::get().applyRepresentation( _chains, _representation, _flag );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Model::Chain *>						  _chains = std::unordered_set<Model::Chain *>();
		const Model::Representation::InstantiatedRepresentation & _representation;
		const Model::Representation::MEMBER_FLAG				  _flag;
	};

} // namespace VTX::Action::Chain
#endif
