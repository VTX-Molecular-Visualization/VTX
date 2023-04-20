#include "app/action/instantiated_representation.hpp"
#include "app/model/molecule.hpp"
#include "app/old_app/vtx_app.hpp"

namespace VTX::Action::InstantiatedRepresentation
{
	void ChangeColorMode::execute()
	{
		_instantiatedRepresentation->setColorMode( _colorMode );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeColor::execute()
	{
		std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

		for ( Model::Representation::InstantiatedRepresentation * const representation : _instantiatedRepresentations )
		{
			representation->setColor( _color, false, true );
			molecules.emplace( representation->getConstTarget()->getMolecule() );
		}

		for ( Model::Molecule * const molecule : molecules )
		{
			molecule->refreshColors();
		}

		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeSphereRadius::execute()
	{
		_instantiatedRepresentation->setSphereRadius( _radius );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeCylinderRadius::execute()
	{
		_instantiatedRepresentation->setCylinderRadius( _radius );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeCylindeColorBlendingMode::execute()
	{
		_instantiatedRepresentation->setCylinderColorBlendingMode( _mode );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

	void ChangeRibbonColorMode::execute()
	{
		_instantiatedRepresentation->setRibbonColorMode( _colorMode );
		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void ChangeRibbonColorBlendingMode::execute()
	{
		_instantiatedRepresentation->setRibbonColorBlendingMode( _mode );
		VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
	}

} // namespace VTX::Action::InstantiatedRepresentation
