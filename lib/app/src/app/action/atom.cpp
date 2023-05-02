#include "app/action/atom.hpp"
#include "app/mvc.hpp"
#include "app/component/chemistry/generated_molecule.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/application/selection/selection.hpp"
#include "app/old_app/generic/base_visible.hpp"
#include "app/application/scene.hpp"
#include "app/application/selection/selection_manager.hpp"
#include "app/util/molecule.hpp"
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
			std::map<App::Component::Chemistry::Molecule *, std::vector<uint>> atomIDsPerMolecules
				= std::map<App::Component::Chemistry::Molecule *, std::vector<uint>>();

			for ( Generic::BaseVisible * const visible : _visibles )
			{
				App::Component::Chemistry::Atom * const atom = static_cast<App::Component::Chemistry::Atom *>( visible );
				atomIDsPerMolecules[ atom->getMoleculePtr() ].emplace_back( atom->getIndex() );
			}

			for ( const App::Application::Scene::PairMoleculePtrFloat & sceneMolecule :
				  VTXApp::get().getScene().getMolecules() )
			{
				App::Component::Chemistry::Molecule * const molecule = sceneMolecule.first;

				std::map<App::Component::Chemistry::Molecule *, std::vector<uint>>::iterator it = atomIDsPerMolecules.find( molecule );

				if ( it != atomIDsPerMolecules.end() )
				{
					Util::App::Molecule::soloAtoms( *molecule, it->second, false );
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
			std::map<App::Component::Chemistry::Molecule *, std::vector<App::Component::Chemistry::Atom *>> atomsPerMolecules
				= std::map<App::Component::Chemistry::Molecule *, std::vector<App::Component::Chemistry::Atom *>>();

			for ( Generic::BaseVisible * const visible : _visibles )
			{
				App::Component::Chemistry::Atom * const atom = static_cast<App::Component::Chemistry::Atom *>( visible );
				atomsPerMolecules[ atom->getMoleculePtr() ].emplace_back( atom );
			}

			for ( const std::pair<App::Component::Chemistry::Molecule *, std::vector<App::Component::Chemistry::Atom *>> & pair : atomsPerMolecules )
			{
				for ( App::Component::Chemistry::Atom * const atom : pair.second )
					Util::App::Molecule::show( *atom, _getVisibilityBool( *atom ), true, false, false );

				pair.first->notifyVisibilityChange();
				pair.first->refreshVisibilities();
				pair.first->computeRepresentationTargets();
			}
		}

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void Delete::execute()
	{
		VTX::App::Application::Selection::SelectionManager::get().getSelectionModel().unselectAtom( _atom );

		App::Component::Chemistry::Molecule * const molecule = _atom.getMoleculePtr();
		molecule->removeAtom( _atom.getIndex() );

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

		generatedMolecule->copyFromAtom( _target );
		generatedMolecule->applyTransform( _target.getMoleculePtr()->getTransform() );
		VTXApp::get().getScene().addMolecule( generatedMolecule );
	}

	void Extract::execute()
	{
		VTX::App::Application::Selection::SelectionManager::get().getSelectionModel().clear();

		App::Component::Chemistry::GeneratedMolecule * const generatedMolecule
			= VTX::MVC_MANAGER().instantiateModel<App::Component::Chemistry::GeneratedMolecule>();

		generatedMolecule->extractAtom( _target );
		VTXApp::get().getScene().addMolecule( generatedMolecule );

		VTX::App::Application::Selection::SelectionManager::get().getSelectionModel().selectMolecule( *generatedMolecule );
	}
} // namespace VTX::App::Action::Atom
