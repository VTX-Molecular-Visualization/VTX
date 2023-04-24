#include "app/action/atom.hpp"
#include "app/core/mvc/mvc_manager.hpp"
#include "app/model/generated_molecule.hpp"
#include "app/model/molecule.hpp"
#include "app/model/selection.hpp"
#include "app/old_app/generic/base_visible.hpp"
#include "app/old_app/object3d/scene.hpp"
#include "app/old_app/selection/selection_manager.hpp"
#include "app/old_app/util/molecule.hpp"
#include "app/old_app/vtx_app.hpp"
#include <map>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Atom
{
	void ChangeColor::execute()
	{
		//_atom.setColor( _color );
		//_atom.getMoleculePtr()->setColorMode();
	}

	void ChangeVisibility::execute()
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

	void Delete::execute()
	{
		VTX::Selection::SelectionManager::get().getSelectionModel().unselectAtom( _atom );

		Model::Molecule * const molecule = _atom.getMoleculePtr();
		molecule->removeAtom( _atom.getIndex() );

		if ( molecule->isEmpty() )
		{
			VTXApp::get().getScene().removeMolecule( molecule );
			VTX::Core::MVC::MvcManager::get().deleteModel( molecule );
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
		Model::GeneratedMolecule * generatedMolecule
			= VTX::Core::MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

		generatedMolecule->copyFromAtom( _target );
		generatedMolecule->applyTransform( _target.getMoleculePtr()->getTransform() );
		VTXApp::get().getScene().addMolecule( generatedMolecule );
	}

	void Extract::execute()
	{
		VTX::Selection::SelectionManager::get().getSelectionModel().clear();

		Model::GeneratedMolecule * const generatedMolecule
			= VTX::Core::MVC::MvcManager::get().instantiateModel<Model::GeneratedMolecule>();

		generatedMolecule->extractAtom( _target );
		VTXApp::get().getScene().addMolecule( generatedMolecule );

		VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecule( *generatedMolecule );
	}
} // namespace VTX::App::Action::Atom
