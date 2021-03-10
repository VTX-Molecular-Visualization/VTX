#include "instantiated_representation.hpp"
#include "generic/base_representable.hpp"
#include "model/molecule.hpp"
#include "model/secondary_structure.hpp"
#include "representation/representation_manager.hpp"
#include "setting.hpp"
#include "vtx_app.hpp"

namespace VTX::Model::Representation
{
	InstantiatedRepresentation::InstantiatedRepresentation( BaseRepresentation * const p_linkedRepresentation ) :
		BaseModel( ID::Model::MODEL_INTANTIATED_REPRESENTATION ), _linkedRepresentation( p_linkedRepresentation ),
		_color( Generic::OverridableParameter<Color::Rgb>( _linkedRepresentation->getColor() ) ),
		_colorMode( Generic::OverridableParameter( _linkedRepresentation->getColorMode() ) ),
		_ssColorMode( Generic::OverridableParameter( _linkedRepresentation->getSecondaryStructureColorMode() ) ),
		_sphereData( Generic::OverridableParameter( _linkedRepresentation->getSphereData() ) ),
		_cylinderData( Generic::OverridableParameter( _linkedRepresentation->getCylinderData() ) ),
		_ribbonData( Generic::OverridableParameter( _linkedRepresentation->getRibbonData() ) ) {};

	InstantiatedRepresentation::InstantiatedRepresentation( const InstantiatedRepresentation * const p_source ) :
		InstantiatedRepresentation( p_source->_linkedRepresentation )
	{
		if ( p_source->_color.isOverrided() )
			_color.setValue( _linkedRepresentation->getColor() );

		if ( p_source->_colorMode.isOverrided() )
			_colorMode.setValue( _linkedRepresentation->getColorMode() );

		_target = p_source->_target;
	};

	void InstantiatedRepresentation::setColorMode( const Generic::COLOR_MODE & p_colorMode )
	{
		_colorMode.setValue( p_colorMode );
		_updateTargets( VTX::Representation::MoleculeComputationFlag::ColorBuffer );

		_notifyDataChanged();
	}

	void InstantiatedRepresentation::setColor( const Color::Rgb & p_color )
	{
		_color.setValue( p_color );
		_updateTargets( VTX::Representation::MoleculeComputationFlag::ColorBuffer );

		_notifyDataChanged();
	}

	const Generic::SECONDARY_STRUCTURE_COLOR_MODE & InstantiatedRepresentation::getSecondaryStructureColorMode() const
	{
		return _ssColorMode.getValue();
	}
	void InstantiatedRepresentation::setSecondaryStructureColorMode(
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode )
	{
		_ssColorMode.setValue( p_colorMode );
		_updateTargets( VTX::Representation::MoleculeComputationFlag::SecondaryStructure );

		_notifyDataChanged();
	}

	void InstantiatedRepresentation::setSphereFixedRadius( const float p_radius )
	{
		_sphereData.getValue()._radiusFixed = p_radius;
		_notifyDataChanged();
	}
	void InstantiatedRepresentation::setSphereAddRadius( const float p_radius )
	{
		_sphereData.getValue()._radiusAdd = p_radius;
		_notifyDataChanged();
	}
	void InstantiatedRepresentation::setCylinderRadius( const float p_radius )
	{
		_cylinderData.getValue()._radius = p_radius;
		_notifyDataChanged();
	}

	void InstantiatedRepresentation::_updateTargets( const VTX::Representation::MoleculeComputationFlag & p_flag )
	{
		if ( int( p_flag & VTX::Representation::MoleculeComputationFlag::Targets ) != 0 )
			_target->getMolecule()->computeRepresentationTargets();

		if ( int( p_flag & VTX::Representation::MoleculeComputationFlag::ColorBuffer ) != 0 )
		{
			_target->getMolecule()->computeColorBuffer();
			_target->getMolecule()->getSecondaryStructure().refreshColors();
		}

		if ( int( p_flag & VTX::Representation::MoleculeComputationFlag::SecondaryStructure ) != 0 )
			_target->getMolecule()->getSecondaryStructure().refreshColors();
	}

} // namespace VTX::Model::Representation
