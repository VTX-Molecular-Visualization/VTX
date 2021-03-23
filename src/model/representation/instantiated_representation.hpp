#ifndef __VTX_MODEL_INSTANTIATED_REPRESENTATION__
#define __VTX_MODEL_INSTANTIATED_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "generic/base_objectoverride.hpp"
#include "id.hpp"
#include "model/base_model.hpp"
#include "model/secondary_structure.hpp"
#include "representation.hpp"
#include "representation_data.hpp"
#include "representation_enum.hpp"

namespace VTX::Model
{
	class Molecule;
	class SecondaryStructure;

} // namespace VTX::Model
namespace VTX::Generic
{
	class BaseRepresentable;
}

namespace VTX::Model::Representation
{
	class InstantiatedRepresentation : public BaseModel, public Generic::BaseObjectOverride
	{
		VTX_MODEL

	  public:
		~InstantiatedRepresentation() {}

		const Generic::BaseRepresentable * const getTarget() const;
		Generic::BaseRepresentable * const		 getTarget();
		void									 setTarget( Generic::BaseRepresentable * p_target );

		const std::string & getName() const { return _linkedRepresentation->getName(); };

		const Color::Rgb & getColor() const { return _color.getValue(); }
		void setColor( const Color::Rgb & p_color, const bool p_recomputeBuffers = true, const bool p_notify = true );
		const Generic::COLOR_MODE & getColorMode() const { return _colorMode.getValue(); }
		void						setColorMode( const Generic::COLOR_MODE & p_colorMode,
												  const bool				  p_recomputeBuffers = true,
												  const bool				  p_notify			 = true );

		const Generic::SECONDARY_STRUCTURE_COLOR_MODE & getSecondaryStructureColorMode() const;
		void setSecondaryStructureColorMode( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode,
											 const bool										 p_recomputeBuffers = true,
											 const bool										 p_notify = true );

		const VTX::Representation::FlagDataTargeted & getFlagDataTargeted() const
		{
			return _linkedRepresentation->getFlagDataTargeted();
		};

		bool			   hasToDrawSphere() const { return _linkedRepresentation->hasToDrawSphere(); };
		const SphereData & getSphereData() const { return _sphereData.getValue(); };
		void			   setSphereRadius( const float p_radius, const bool p_notify = true );

		bool				 hasToDrawCylinder() const { return _linkedRepresentation->hasToDrawCylinder(); };
		const CylinderData & getCylinderData() const { return _cylinderData.getValue(); };
		void				 setCylinderRadius( const float p_radius, const bool p_notify = true );

		bool			   hasToDrawRibbon() const { return _linkedRepresentation->hasToDrawRibbon(); };
		const RibbonData & getRibbonData() const { return _ribbonData.getValue(); };

		void applyData( const InstantiatedRepresentation & p_source,
						const MEMBER_FLAG &				   p_flag,
						const bool						   p_recomputeBuffers = true );

		const BaseRepresentation * const getLinkedRepresentation() const { return _linkedRepresentation; }
		BaseRepresentation * const		 getLinkedRepresentation() { return _linkedRepresentation; }

	  protected:
		InstantiatedRepresentation( BaseRepresentation * const p_linkedRepresentation );
		InstantiatedRepresentation( const InstantiatedRepresentation * const p_source );

		BaseRepresentation * const _linkedRepresentation;

		Generic::BaseRepresentable * _target = nullptr;

		Generic::OverridableParameter<Color::Rgb>							   _color;
		Generic::OverridableParameter<Generic::COLOR_MODE>					   _colorMode;
		Generic::OverridableParameter<Generic::SECONDARY_STRUCTURE_COLOR_MODE> _ssColorMode;

		Generic::OverridableParameter<SphereData>	_sphereData;
		Generic::OverridableParameter<CylinderData> _cylinderData;
		Generic::OverridableParameter<RibbonData>	_ribbonData;

		void _updateTarget( const VTX::Representation::MoleculeComputationFlag & p_flag );
	};

} // namespace VTX::Model::Representation
#endif
