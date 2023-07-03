#include "app/old/action/atom.hpp"
#include "app/old/application/scene.hpp"
#include "app/old/application/selection/selection.hpp"
#include "app/old/application/selection/selection_manager.hpp"
#include "app/old/component/chemistry/generated_molecule.hpp"
#include "app/old/component/chemistry/molecule.hpp"
#include "app/old/mvc.hpp"
#include "app/old/component/generic/base_visible.hpp"
#include "app/old/vtx_app.hpp"
#include "app/old/util/molecule.hpp"
#include <map>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Old::Action::Atom
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
			std::map<App::Old::Component::Chemistry::Molecule *, std::vector<uint>> atomIDsPerMolecules
				= std::map<App::Old::Component::Chemistry::Molecule *, std::vector<uint>>();

			for ( Component::Generic::BaseVisible * const visible : _visibles )
			{
				App::Old::Component::Chemistry::Atom * const atom
					= static_cast<App::Old::Component::Chemistry::Atom *>( visible );
				atomIDsPerMolecules[ atom->getMoleculePtr() ].emplace_back( atom->getIndex() );
			}

			for ( const App::Old::Application::Scene::PairMoleculePtrFloat & sceneMolecule :
				  App::Old::VTXApp::get().getScene().getMolecules() )
			{
				App::Old::Component::Chemistry::Molecule * const molecule = sceneMolecule.first;

				std::map<App::Old::Component::Chemistry::Molecule *, std::vector<uint>>::iterator it
					= atomIDsPerMolecules.find( molecule );

				if ( it != atomIDsPerMolecules.end() )
				{
					Util::App::Old::Molecule::soloAtoms( *molecule, it->second, false );
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
			std::map<App::Old::Component::Chemistry::Molecule *, std::vector<App::Old::Component::Chemistry::Atom *>>
				atomsPerMolecules
				= std::map<App::Old::Component::Chemistry::Molecule *, std::vector<App::Old::Component::Chemistry::Atom *>>();

			for ( Component::Generic::BaseVisible * const visible : _visibles )
			{
				App::Old::Component::Chemistry::Atom * const atom
					= static_cast<App::Old::Component::Chemistry::Atom *>( visible );
				atomsPerMolecules[ atom->getMoleculePtr() ].emplace_back( atom );
			}

			for ( const std::pair<App::Old::Component::Chemistry::Molecule *,
								  std::vector<App::Old::Component::Chemistry::Atom *>> & pair : atomsPerMolecules )
			{
				for ( App::Old::Component::Chemistry::Atom * const atom : pair.second )
					Util::App::Old::Molecule::show( *atom, _getVisibilityBool( *atom ), true, false, false );

				pair.first->notifyVisibilityChange();
				pair.first->refreshVisibilities();
				pair.first->computeRepresentationTargets();
			}
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void Delete::execute()
	{
		VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().unselectAtom( _atom );

		App::Old::Component::Chemistry::Molecule * const molecule = _atom.getMoleculePtr();
		molecule->removeAtom( _atom.getIndex() );

		if ( molecule->isEmpty() )
		{
			App::Old::VTXApp::get().getScene().removeMolecule( molecule );
			VTX::MVC_MANAGER().deleteModel( molecule );
		}
		else
		{
			molecule->refreshStructure();
			molecule->computeAllRepresentationData();
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_SELECTION_UPDATED;
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void Copy::execute()
	{
		App::Old::Component::Chemistry::GeneratedMolecule * generatedMolecule
			= VTX::MVC_MANAGER().instantiateModel<App::Old::Component::Chemistry::GeneratedMolecule>();

		generatedMolecule->copyFromAtom( _target );
		generatedMolecule->applyTransform( _target.getMoleculePtr()->getTransform() );
		App::Old::VTXApp::get().getScene().addMolecule( generatedMolecule );
	}

	void Extract::execute()
	{
		VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().clear();

		App::Old::Component::Chemistry::GeneratedMolecule * const generatedMolecule
			= VTX::MVC_MANAGER().instantiateModel<App::Old::Component::Chemistry::GeneratedMolecule>();

		generatedMolecule->extractAtom( _target );
		App::Old::VTXApp::get().getScene().addMolecule( generatedMolecule );

		VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().selectMolecule(
			*generatedMolecule );
	}
} // namespace VTX::App::Old::Action::Atom
