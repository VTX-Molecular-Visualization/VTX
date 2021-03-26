#include "instantiated_representation.hpp"
#include "generic/base_representable.hpp"
#include "model/molecule.hpp"
#include "model/secondary_structure.hpp"
#include "mvc/mvc_manager.hpp"
#include "representation/representation_manager.hpp"
#include "setting.hpp"
#include "vtx_app.hpp"

namespace VTX::Model::Representation
{
	InstantiatedRepresentation * const InstantiatedRepresentation::instantiateCopy(
		const InstantiatedRepresentation * const p_source )
	{
		InstantiatedRepresentation * const res
			= MVC::MvcManager::get().instantiateModel<InstantiatedRepresentation>( p_source->_linkedRepresentation );

		if ( p_source->_color.isOverrided() )
			res->_color.setValue( p_source->_color.getValue() );

		if ( p_source->_colorMode.isOverrided() )
			res->_colorMode.setValue( p_source->_colorMode.getValue() );

		if ( p_source->_ssColorMode.isOverrided() )
			res->_ssColorMode.setValue( p_source->_ssColorMode.getValue() );

		if ( p_source->_sphereData.isOverrided() )
			res->_sphereData.setValue( p_source->_sphereData.getValue() );

		if ( p_source->_cylinderData.isOverrided() )
			res->_cylinderData.setValue( p_source->_cylinderData.getValue() );

		if ( p_source->_ribbonData.isOverrided() )
			res->_ribbonData.setValue( p_source->_ribbonData.getValue() );

		res->setTarget( p_source->_target );

		return res;
	}

	InstantiatedRepresentation::InstantiatedRepresentation( const BaseRepresentation * const p_linkedRepresentation ) :
		BaseModel( ID::Model::MODEL_INTANTIATED_REPRESENTATION ), _linkedRepresentation( p_linkedRepresentation ),
		_color( Generic::OverridableParameter<Color::Rgb>( _linkedRepresentation->getColor() ) ),
		_colorMode( Generic::OverridableParameter( _linkedRepresentation->getColorMode() ) ),
		_ssColorMode( Generic::OverridableParameter( _linkedRepresentation->getSecondaryStructureColorMode() ) ),
		_sphereData( Generic::OverridableParameter( _linkedRepresentation->getSphereData() ) ),
		_cylinderData( Generic::OverridableParameter( _linkedRepresentation->getCylinderData() ) ),
		_ribbonData( Generic::OverridableParameter( _linkedRepresentation->getRibbonData() ) ) {};

	void InstantiatedRepresentation::setLinkedRepresentation( const BaseRepresentation * const p_linkedRepresentation )
	{
		_linkedRepresentation = p_linkedRepresentation;

		_color		  = Generic::OverridableParameter<Color::Rgb>( _linkedRepresentation->getColor() );
		_colorMode	  = Generic::OverridableParameter( _linkedRepresentation->getColorMode() );
		_ssColorMode  = Generic::OverridableParameter( _linkedRepresentation->getSecondaryStructureColorMode() );
		_sphereData	  = Generic::OverridableParameter( _linkedRepresentation->getSphereData() );
		_cylinderData = Generic::OverridableParameter( _linkedRepresentation->getCylinderData() );
		_ribbonData	  = Generic::OverridableParameter( _linkedRepresentation->getRibbonData() );
	}

	const Generic::BaseRepresentable * const InstantiatedRepresentation::getTarget() const { return _target; }
	Generic::BaseRepresentable * const		 InstantiatedRepresentation::getTarget() { return _target; }
	void InstantiatedRepresentation::setTarget( Generic::BaseRepresentable * p_target ) { _target = p_target; }

	void InstantiatedRepresentation::setColorMode( const Generic::COLOR_MODE & p_colorMode,
												   const bool				   p_recomputeBuffers,
												   const bool				   p_notify )
	{
		_colorMode.setValue( p_colorMode );

		if ( p_recomputeBuffers )
			_updateTarget( VTX::Representation::MoleculeComputationFlag::ColorBuffer );

		if ( p_notify )
			_notifyDataChanged();
	}

	void InstantiatedRepresentation::setColor( const Color::Rgb & p_color,
											   const bool		  p_recomputeBuffers,
											   const bool		  p_notify )
	{
		_color.setValue( p_color );

		if ( p_recomputeBuffers )
			_updateTarget( VTX::Representation::MoleculeComputationFlag::ColorBuffer );

		if ( p_notify )
			_notifyDataChanged();
	}

	const Generic::SECONDARY_STRUCTURE_COLOR_MODE & InstantiatedRepresentation::getSecondaryStructureColorMode() const
	{
		return _ssColorMode.getValue();
	}
	void InstantiatedRepresentation::setSecondaryStructureColorMode(
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode,
		const bool										p_recomputeBuffers,
		const bool										p_notify )
	{
		_ssColorMode.setValue( p_colorMode );

		if ( p_recomputeBuffers )
			_updateTarget( VTX::Representation::MoleculeComputationFlag::SecondaryStructure );

		if ( p_notify )
			_notifyDataChanged();
	}

	void InstantiatedRepresentation::setSphereRadius( const float p_radius, const bool p_notify )
	{
		bool dataChanged = false;

		switch ( _linkedRepresentation->getRepresentationType() )
		{
		case Generic::REPRESENTATION::BALL_AND_STICK:
		case Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON:
			if ( _sphereData.getValue()._radiusFixed != p_radius )
			{
				_sphereData.getValue()._radiusFixed = p_radius;
				if ( p_radius < _cylinderData.getValue()._radius )
					_cylinderData.getValue()._radius = p_radius;

				dataChanged = true;
			}
			break;

		case Generic::REPRESENTATION::SAS:
		case Generic::REPRESENTATION::VAN_DER_WAALS:
			dataChanged						  = _sphereData.getValue()._radiusAdd != p_radius;
			_sphereData.getValue()._radiusAdd = p_radius;
			break;

		case Generic::REPRESENTATION::STICK:
		case Generic::REPRESENTATION::STICK_AND_CARTOON:
		case Generic::REPRESENTATION::CARTOON:
		case Generic::REPRESENTATION::TRACE: break;
		}

		if ( p_notify && dataChanged )
			_notifyDataChanged();
	}

	void InstantiatedRepresentation::setCylinderRadius( const float p_radius, const bool p_notify )
	{
		bool dataChanged = false;

		switch ( _linkedRepresentation->getRepresentationType() )
		{
		case Generic::REPRESENTATION::BALL_AND_STICK:
		case Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON:
			if ( _cylinderData.getValue()._radius != p_radius )
			{
				_cylinderData.getValue()._radius = p_radius;
				if ( p_radius > getSphereData()._radiusFixed )
					_sphereData.getValue()._radiusFixed = p_radius;

				dataChanged = true;
			}

			break;

		case Generic::REPRESENTATION::STICK:
		case Generic::REPRESENTATION::STICK_AND_CARTOON:
		case Generic::REPRESENTATION::TRACE:
			if ( _cylinderData.getValue()._radius != p_radius )
			{
				_sphereData.getValue()._radiusFixed = p_radius;
				_cylinderData.getValue()._radius	= p_radius;

				dataChanged = true;
			}
			break;

		case Generic::REPRESENTATION::VAN_DER_WAALS:
		case Generic::REPRESENTATION::CARTOON:
		case Generic::REPRESENTATION::SAS: break;
		}

		if ( p_notify && dataChanged )
			_notifyDataChanged();
	}

	void InstantiatedRepresentation::applyData( const InstantiatedRepresentation & p_source,
												const MEMBER_FLAG &				   p_flag,
												const bool						   p_recomputeBuffers )
	{
		if ( p_flag & MEMBER_FLAG::SPHERE_RADIUS_FIXED && p_source.hasToDrawSphere() )
			_sphereData.getValue()._radiusFixed = p_source.getSphereData()._radiusFixed;

		if ( p_flag & MEMBER_FLAG::SPHERE_RADIUS_ADD && p_source.hasToDrawSphere() )
			_sphereData.getValue()._radiusAdd = p_source.getSphereData()._radiusAdd;

		if ( p_flag & MEMBER_FLAG::CYLINDER_RADIUS && p_source.hasToDrawCylinder() )
			_cylinderData.getValue()._radius = p_source.getCylinderData()._radius;

		if ( p_flag & MEMBER_FLAG::COLOR_MODE )
			setColorMode( p_source.getColorMode(), p_recomputeBuffers, false );

		if ( p_flag & MEMBER_FLAG::SS_COLOR_MODE )
			setSecondaryStructureColorMode( p_source.getSecondaryStructureColorMode(), p_recomputeBuffers, false );

		if ( p_flag & MEMBER_FLAG::COLOR )
			setColor( p_source.getColor(), p_recomputeBuffers, false );

		_notifyDataChanged();
	}

	void InstantiatedRepresentation::_updateTarget( const VTX::Representation::MoleculeComputationFlag & p_flag )
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
