#ifndef __VTX_ACTION_RESIDUE__
#define __VTX_ACTION_RESIDUE__

#include "old_app/event/event.hpp"
#include "old_app/event/event_manager.hpp"
#include "old_app/model/chain.hpp"
#include "old_app/model/generated_molecule.hpp"
#include "old_app/model/molecule.hpp"
#include "old_app/model/representation/instantiated_representation.hpp"
#include "old_app/model/representation/representation_library.hpp"
#include "old_app/model/residue.hpp"
#include "old_app/model/selection.hpp"
#include "old_app/mvc/mvc_manager.hpp"
#include "old_app/object3d/scene.hpp"
#include "old_app/representation/representation_manager.hpp"
#include "old_app/selection/selection_manager.hpp"
#include "old_app/util/molecule.hpp"
#include "visible.hpp"
#include <unordered_set>

namespace VTX::Action::Residue
{
	class ChangeColor : public BaseAction
	{
	  public:
		explicit ChangeColor( Model::Residue & p_residue, const Color::Rgba & p_color ) :
			_color( p_color ), _residues { &p_residue }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeColor( const std::unordered_set<Model::Residue *> & p_residues, const Color::Rgba & p_color ) :
			_color( p_color ), _residues( p_residues )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
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
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Residue *> _residues;
		const Color::Rgba						   _color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( Model::Residue & p_residue, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_residue, p_mode )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
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

					std::map<Model::Molecule *, std::vector<uint>>::iterator it
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
	};

	class ChangeRepresentationPreset : public BaseAction
	{
	  public:
		explicit ChangeRepresentationPreset( Model::Residue & p_residue, const int p_indexPreset ) :
			_indexPreset( p_indexPreset ), _residues { &p_residue }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit ChangeRepresentationPreset( const std::unordered_set<Model::Residue *> & p_residues,
											 const int									  p_indexPreset ) :
			_indexPreset( p_indexPreset ),
			_residues( p_residues )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			Model::Representation::Representation * const preset
				= Model::Representation::RepresentationLibrary::get().getRepresentation( _indexPreset );

			Representation::RepresentationManager::get().instantiateRepresentations( preset, _residues );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Residue *> _residues;
		const int								   _indexPreset;
	};

	class RemoveRepresentation : public BaseAction
	{
	  public:
		explicit RemoveRepresentation( Model::Residue & p_residue ) : _residues { &p_residue }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit RemoveRepresentation( const std::unordered_set<Model::Residue *> & p_residues ) :
			_residues( p_residues )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			Representation::RepresentationManager::get().removeInstantiatedRepresentations( _residues );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Residue *> _residues;
	};

	class Delete : public BaseAction
	{
	  public:
		explicit Delete( Model::Residue & p_residue ) : _residue( p_residue )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

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
				molecule->refreshStructure();
				molecule->computeAllRepresentationData();
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
		explicit Copy( const Model::Residue & p_target ) : _target( p_target )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override
		{
			Model::GeneratedMolecule * generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->copyFromResidue( _target );
			generatedMolecule->applyTransform( _target.getMoleculePtr()->getTransform() );

			VTXApp::get().getScene().addMolecule( generatedMolecule );
		}

	  private:
		const Model::Residue & _target;
	};

	class Extract : public BaseAction
	{
	  public:
		explicit Extract( const Model::Residue & p_target ) : _target( p_target )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override
		{
			VTX::Selection::SelectionManager::get().getSelectionModel().clear();

			Model::GeneratedMolecule * const generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->extractResidue( _target );
			VTXApp::get().getScene().addMolecule( generatedMolecule );

			VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecule( *generatedMolecule );
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
			_flag( p_flag ), _residues( p_residues )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			Representation::RepresentationManager::get().applyRepresentation( _residues, _representation, _flag );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const std::unordered_set<Model::Residue *>				  _residues;
		const Model::Representation::InstantiatedRepresentation & _representation;
		const Model::Representation::MEMBER_FLAG				  _flag;
	};
} // namespace VTX::Action::Residue
#endif
