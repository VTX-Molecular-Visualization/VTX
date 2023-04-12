#ifndef __VTX_ACTION_ATOM__
#define __VTX_ACTION_ATOM__

#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "model/atom.hpp"
#include "model/generated_molecule.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "util/molecule.hpp"
#include "visible.hpp"
#include "vtx_app.hpp"
#include <set>

namespace VTX::Action::Atom
{
	class ChangeColor : public BaseAction
	{
	  public:
		explicit ChangeColor( Model::Atom & p_atom, const Color::Rgba & p_color ) : _atom( p_atom ), _color( p_color )
		{
		}

		virtual void execute() override
		{
			//_atom.setColor( _color );
			//_atom.getMoleculePtr()->setColorMode();
		}

	  private:
		Model::Atom &	  _atom;
		const Color::Rgba _color;
	};

	class ChangeVisibility : public Visible::ChangeVisibility
	{
	  public:
		explicit ChangeVisibility( Model::Atom & p_atom, const VISIBILITY_MODE p_mode ) :
			Visible::ChangeVisibility( p_atom, p_mode )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			if ( _mode == VISIBILITY_MODE::SOLO )
			{
				std::map<Model::Molecule *, std::vector<uint>> atomIDsPerMolecules
					= std::map<Model::Molecule *, std::vector<uint>>();

				for ( Generic::BaseVisible * const visible : _visibles )
				{
					Model::Atom * const atom = static_cast<Model::Atom *>( visible );
					atomIDsPerMolecules[ atom->getMoleculePtr() ].emplace_back( atom->getIndex() );
				}

				for ( const Object3D::Scene::PairMoleculePtrFloat & sceneMolecule :
					  VTXApp::get().getScene().getMolecules() )
				{
					Model::Molecule * const molecule = sceneMolecule.first;

					std::map<Model::Molecule *, std::vector<uint>>::iterator it = atomIDsPerMolecules.find( molecule );

					if ( it != atomIDsPerMolecules.end() )
					{
						Util::Molecule::soloAtoms( *molecule, it->second, false );
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
				std::map<Model::Molecule *, std::vector<Model::Atom *>> atomsPerMolecules
					= std::map<Model::Molecule *, std::vector<Model::Atom *>>();

				for ( Generic::BaseVisible * const visible : _visibles )
				{
					Model::Atom * const atom = static_cast<Model::Atom *>( visible );
					atomsPerMolecules[ atom->getMoleculePtr() ].emplace_back( atom );
				}

				for ( const std::pair<Model::Molecule *, std::vector<Model::Atom *>> & pair : atomsPerMolecules )
				{
					for ( Model::Atom * const atom : pair.second )
						Util::Molecule::show( *atom, _getVisibilityBool( *atom ), true, false, false );

					pair.first->notifyVisibilityChange();
					pair.first->refreshVisibilities();
					pair.first->computeRepresentationTargets();
				}
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}
	};

	class Delete : public BaseAction
	{
	  public:
		explicit Delete( Model::Atom & p_atom ) : _atom( p_atom )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			VTX::Selection::SelectionManager::get().getSelectionModel().unselectAtom( _atom );

			Model::Molecule * const molecule = _atom.getMoleculePtr();
			molecule->removeAtom( _atom.getIndex() );

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
		Model::Atom & _atom;
	};

	class Copy : public BaseAction
	{
	  public:
		explicit Copy( const Model::Atom & p_target ) : _target( p_target )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override
		{
			Model::GeneratedMolecule * generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->copyFromAtom( _target );
			generatedMolecule->applyTransform( _target.getMoleculePtr()->getTransform() );
			VTXApp::get().getScene().addMolecule( generatedMolecule );
		}

	  private:
		const Model::Atom & _target;
	};

	class Extract : public BaseAction
	{
	  public:
		explicit Extract( const Model::Atom & p_target ) : _target( p_target )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		virtual void execute() override
		{
			VTX::Selection::SelectionManager::get().getSelectionModel().clear();

			Model::GeneratedMolecule * const generatedMolecule
				= MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

			generatedMolecule->extractAtom( _target );
			VTXApp::get().getScene().addMolecule( generatedMolecule );

			VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecule( *generatedMolecule );
		}

	  private:
		const Model::Atom & _target;
	};
} // namespace VTX::Action::Atom
#endif
