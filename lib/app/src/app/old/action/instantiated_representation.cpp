#include "app/old/action/instantiated_representation.hpp"
#include "app/old/component/chemistry/molecule.hpp"
#include "app/old/vtx_app.hpp"

namespace VTX::App::Old::Action::InstantiatedRepresentation
{
	void ChangeColorMode::execute()
	{
		_instantiatedRepresentation->setColorMode( _colorMode );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeColor::execute()
	{
		std::unordered_set<App::Old::Component::Chemistry::Molecule *> molecules = std::unordered_set<App::Old::Component::Chemistry::Molecule *>();

		for ( App::Old::Application::Representation::InstantiatedRepresentation * const representation : _instantiatedRepresentations )
		{
			representation->setColor( _color, false, true );
			molecules.emplace( representation->getConstTarget()->getMolecule() );
		}

		for ( App::Old::Component::Chemistry::Molecule * const molecule : molecules )
		{
			molecule->refreshColors();
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeSphereRadius::execute()
	{
		_instantiatedRepresentation->setSphereRadius( _radius );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeCylinderRadius::execute()
	{
		_instantiatedRepresentation->setCylinderRadius( _radius );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeCylindeColorBlendingMode::execute()
	{
		_instantiatedRepresentation->setCylinderColorBlendingMode( _mode );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeRibbonColorMode::execute()
	{
		_instantiatedRepresentation->setRibbonColorMode( _colorMode );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeRibbonColorBlendingMode::execute()
	{
		_instantiatedRepresentation->setRibbonColorBlendingMode( _mode );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

} // namespace VTX::App::Old::Action::InstantiatedRepresentation
