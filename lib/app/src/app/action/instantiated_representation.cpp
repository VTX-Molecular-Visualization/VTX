#include "app/action/instantiated_representation.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Action::InstantiatedRepresentation
{
	void ChangeColorMode::execute()
	{
		_instantiatedRepresentation->setColorMode( _colorMode );
		App::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeColor::execute()
	{
		std::unordered_set<App::Component::Chemistry::Molecule *> molecules = std::unordered_set<App::Component::Chemistry::Molecule *>();

		for ( App::Application::Representation::InstantiatedRepresentation * const representation : _instantiatedRepresentations )
		{
			representation->setColor( _color, false, true );
			molecules.emplace( representation->getConstTarget()->getMolecule() );
		}

		for ( App::Component::Chemistry::Molecule * const molecule : molecules )
		{
			molecule->refreshColors();
		}

		App::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeSphereRadius::execute()
	{
		_instantiatedRepresentation->setSphereRadius( _radius );
		App::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeCylinderRadius::execute()
	{
		_instantiatedRepresentation->setCylinderRadius( _radius );
		App::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeCylindeColorBlendingMode::execute()
	{
		_instantiatedRepresentation->setCylinderColorBlendingMode( _mode );
		App::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeRibbonColorMode::execute()
	{
		_instantiatedRepresentation->setRibbonColorMode( _colorMode );
		App::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeRibbonColorBlendingMode::execute()
	{
		_instantiatedRepresentation->setRibbonColorBlendingMode( _mode );
		App::VTXApp::get().MASK |= Render::VTX_MASK_UNIFORM_UPDATED;
	}

} // namespace VTX::App::Action::InstantiatedRepresentation
