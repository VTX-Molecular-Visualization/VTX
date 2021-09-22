#include "instantiated_representation.hpp"
#include "event/event.hpp"
#include "generic/base_representable.hpp"
#include "model/molecule.hpp"
#include "model/secondary_structure.hpp"
#include "mvc/mvc_manager.hpp"
#include "representation/representation_manager.hpp"
#include "setting.hpp"
#include "tool/logger.hpp"
#include "vtx_app.hpp"
#include <string>

namespace VTX::Model::Representation
{
	void InstantiatedRepresentation::copy( const InstantiatedRepresentation & p_source )
	{
		if ( p_source._color.isOverrided() )
			_color.setValue( p_source._color.getValue() );

		if ( p_source._colorMode.isOverrided() )
			_colorMode.setValue( p_source._colorMode.getValue() );

		if ( p_source._ssColorMode.isOverrided() )
			_ssColorMode.setValue( p_source._ssColorMode.getValue() );

		if ( p_source._sphereData.isOverrided() )
			_sphereData.setValue( p_source._sphereData.getValue() );

		if ( p_source._cylinderData.isOverrided() )
			_cylinderData.setValue( p_source._cylinderData.getValue() );

		if ( p_source._ribbonData.isOverrided() )
			_ribbonData.setValue( p_source._ribbonData.getValue() );

		_target = p_source._target;
	}

	InstantiatedRepresentation::InstantiatedRepresentation() : BaseModel( ID::Model::MODEL_INTANTIATED_REPRESENTATION )
	{
	}

	InstantiatedRepresentation::InstantiatedRepresentation( const Representation * const p_linkedRepresentation ) :
		BaseModel( ID::Model::MODEL_INTANTIATED_REPRESENTATION ), _linkedRepresentation( p_linkedRepresentation ),
		_color( Generic::OverridableParameter<Color::Rgb>( _linkedRepresentation->getColor() ) ),
		_colorMode( Generic::OverridableParameter( _linkedRepresentation->getData().getColorMode() ) ),
		_ssColorMode(
			Generic::OverridableParameter( _linkedRepresentation->getData().getSecondaryStructureColorMode() ) ),
		_sphereData( Generic::OverridableParameter( _linkedRepresentation->getData().getSphereData() ) ),
		_cylinderData( Generic::OverridableParameter( _linkedRepresentation->getData().getCylinderData() ) ),
		_ribbonData( Generic::OverridableParameter( _linkedRepresentation->getData().getRibbonData() ) )
	{
		_registerEvent( Event::Global::MOLECULE_COLOR_CHANGE );
	}

	InstantiatedRepresentation::~InstantiatedRepresentation() {}

	void InstantiatedRepresentation::onLinkedRepresentationChange( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::REPRESENTATION_TYPE_CHANGE || p_event->name == Event::Model::DATA_CHANGE )
		{
			if ( _target != nullptr )
				_updateTarget( VTX::Representation::MoleculeComputationFlag::ALL );
		}

		_notifyViews( new Event::VTXEvent( *p_event ) );
	}

	void InstantiatedRepresentation::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::MOLECULE_COLOR_CHANGE )
		{
			const bool useMoleculeColor
				= _colorMode.getValue() == Generic::COLOR_MODE::ATOM_PROTEIN
				  || _colorMode.getValue() == Generic::COLOR_MODE::PROTEIN
				  || _ssColorMode.getValue() == Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN;

			if ( useMoleculeColor )
				_notifyDataChanged();
		}
	}

	void InstantiatedRepresentation::refreshSourceValues()
	{
		_color.resetSource( &_linkedRepresentation->getColor() );
		_colorMode.resetSource( &_linkedRepresentation->getData().getColorMode() );
		_ssColorMode.resetSource( &_linkedRepresentation->getData().getSecondaryStructureColorMode() );
		_sphereData.resetSource( &_linkedRepresentation->getData().getSphereData() );
		_cylinderData.resetSource( &_linkedRepresentation->getData().getCylinderData() );
		_ribbonData.resetSource( &_linkedRepresentation->getData().getRibbonData() );
	}

	void InstantiatedRepresentation::setLinkedRepresentation( const Representation * const p_linkedRepresentation,
															  const bool				   p_eraseOverride )
	{
		_linkedRepresentation = p_linkedRepresentation;

		if ( p_eraseOverride || !_color.isOverrided() )
			_color = Generic::OverridableParameter<Color::Rgb>( _linkedRepresentation->getColor() );
		if ( p_eraseOverride || !_colorMode.isOverrided() )
			_colorMode = Generic::OverridableParameter( _linkedRepresentation->getData().getColorMode() );
		if ( p_eraseOverride || !_ssColorMode.isOverrided() )
			_ssColorMode
				= Generic::OverridableParameter( _linkedRepresentation->getData().getSecondaryStructureColorMode() );
		if ( p_eraseOverride || !_sphereData.isOverrided() )
			_sphereData = Generic::OverridableParameter( _linkedRepresentation->getData().getSphereData() );
		if ( p_eraseOverride || !_cylinderData.isOverrided() )
			_cylinderData = Generic::OverridableParameter( _linkedRepresentation->getData().getCylinderData() );
		if ( p_eraseOverride || !_ribbonData.isOverrided() )
			_ribbonData = Generic::OverridableParameter( _linkedRepresentation->getData().getRibbonData() );
	}

	const Generic::BaseRepresentable * const InstantiatedRepresentation::getConstTarget() const { return _target; }
	Generic::BaseRepresentable * const		 InstantiatedRepresentation::getTarget() const { return _target; };
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
			// !V0.1
			// case Generic::REPRESENTATION::TRACE:
			break;
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
			// !V0.1
			// case Generic::REPRESENTATION::TRACE:
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
												const bool						   p_recomputeBuffers,
												const bool						   p_notify )
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

		if ( p_notify )
			_notifyDataChanged();
	}

	MEMBER_FLAG InstantiatedRepresentation::getOverridedMembersFlag() const
	{
		MEMBER_FLAG res = MEMBER_FLAG::NONE;

		if ( hasToDrawSphere() && _sphereData.isOverrided() )
		{
			res = MEMBER_FLAG( res | MEMBER_FLAG::SPHERE_RADIUS_ADD );
			res = MEMBER_FLAG( res | MEMBER_FLAG::SPHERE_RADIUS_FIXED );
		}

		if ( hasToDrawCylinder() && _cylinderData.isOverrided() )
			res = MEMBER_FLAG( res | MEMBER_FLAG::CYLINDER_RADIUS );

		if ( _colorMode.isOverrided() )
			res = MEMBER_FLAG( res | MEMBER_FLAG::COLOR_MODE );

		if ( _ssColorMode.isOverrided() )
			res = MEMBER_FLAG( res | MEMBER_FLAG::SS_COLOR_MODE );

		if ( _color.isOverrided() )
			res = MEMBER_FLAG( res | MEMBER_FLAG::COLOR );

		return res;
	}
	bool InstantiatedRepresentation::isMemberOverrided( const MEMBER_FLAG & p_member ) const
	{
		switch ( p_member )
		{
		case SPHERE_RADIUS_FIXED: return _sphereData.isOverrided(); break;
		case SPHERE_RADIUS_ADD: return _sphereData.isOverrided(); break;
		case CYLINDER_RADIUS: return _cylinderData.isOverrided(); break;
		case COLOR: return _color.isOverrided(); break;
		case COLOR_MODE: return _colorMode.isOverrided(); break;
		case SS_COLOR_MODE: return _ssColorMode.isOverrided(); break;
		default:
			VTX_WARNING( "MEMBER_FLAG " + std::to_string( int( p_member ) )
						 + " not managed in InstantiatedRepresentation::isMemberOverrided." );
			return false;
		}
	}

	void InstantiatedRepresentation::_updateTarget( const VTX::Representation::MoleculeComputationFlag & p_flag )
	{
		if ( _target == nullptr )
		{
			return;
		}

		if ( int( p_flag & VTX::Representation::MoleculeComputationFlag::Targets ) != 0 )
		{
			_target->getMolecule()->computeRepresentationTargets();
		}

		if ( int( p_flag & VTX::Representation::MoleculeComputationFlag::ColorBuffer ) != 0 )
		{
			_target->getMolecule()->refreshColors();
		}

		if ( int( p_flag & VTX::Representation::MoleculeComputationFlag::SecondaryStructure ) != 0 )
		{
			_target->getMolecule()->getSecondaryStructure().refreshColors();
		}
	}

} // namespace VTX::Model::Representation
