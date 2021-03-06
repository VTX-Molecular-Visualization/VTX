#include "instantiated_representation.hpp"
#include "generic/base_representable.hpp"
#include "model/molecule.hpp"
#include "representation/representation_manager.hpp"
#include "setting.hpp"
#include "vtx_app.hpp"

namespace VTX::Model::Representation
{
	InstantiatedRepresentation::InstantiatedRepresentation( BaseRepresentation * const p_linkedRepresentation ) :
		BaseModel( ID::Model::MODEL_INTANTIATED_REPRESENTATION ), _linkedRepresentation( p_linkedRepresentation ),
		_color( Generic::OverridableParameter<Color::Rgb>( _linkedRepresentation->getColor() ) ),
		_colorMode( Generic::OverridableParameter( _linkedRepresentation->getColorMode() ) ),
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
	}

	void InstantiatedRepresentation::setColor( const Color::Rgb & p_color )
	{
		_color.setValue( p_color );
		_updateTargets( VTX::Representation::MoleculeComputationFlag::ColorBuffer );
	}

	void InstantiatedRepresentation::setSphereFixedRadius( const float p_radius )
	{
		_sphereData.getValue()._radiusFixed = p_radius;
	}
	void InstantiatedRepresentation::setSphereAddRadius( const float p_radius )
	{
		_sphereData.getValue()._radiusAdd = p_radius;
	}
	void InstantiatedRepresentation::setCylinderRadius( const float p_radius )
	{
		_cylinderData.getValue()._radius = p_radius;
	}

	void InstantiatedRepresentation::_updateTargets( const VTX::Representation::MoleculeComputationFlag & p_flag )
	{
		if ( int( p_flag & VTX::Representation::MoleculeComputationFlag::Targets ) != 0 )
			_target->getMolecule()->computeRepresentationTargets();

		if ( int( p_flag & VTX::Representation::MoleculeComputationFlag::ColorBuffer ) != 0 )
			_target->getMolecule()->computeColorBuffer();
	}

} // namespace VTX::Model::Representation
