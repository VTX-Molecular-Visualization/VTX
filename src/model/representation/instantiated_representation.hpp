#ifndef __VTX_MODEL_INSTANTIATED_REPRESENTATION__
#define __VTX_MODEL_INSTANTIATED_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "generic/base_objectoverride.hpp"
#include "id.hpp"
#include "model/base_model.hpp"
#include "representation.hpp"
#include "representation_data.hpp"
#include "representation_enum.hpp"

namespace VTX::Model
{
	class Molecule;
}
namespace VTX::Generic
{
	class BaseRepresentable;
}

namespace VTX::Model::Representation
{
	class InstantiatedRepresentation : public BaseModel, public Generic::BaseObjectOverride
	{
	  public:
		InstantiatedRepresentation( BaseRepresentation * const p_linkedRepresentation );
		InstantiatedRepresentation( const InstantiatedRepresentation * const p_source );

		~InstantiatedRepresentation() {}

		void setTarget( Generic::BaseRepresentable * p_target ) { _target = p_target; }

		const std::string & getName() const { return _linkedRepresentation->getName(); };

		const Color::Rgb &			getColor() const { return _color.getValue(); }
		void						setColor( const Color::Rgb & p_color );
		const Generic::COLOR_MODE & getColorMode() const { return _colorMode.getValue(); }
		void						setColorMode( const Generic::COLOR_MODE & p_colorMode );

		const VTX::Representation::FlagDataTargeted & getFlagDataTargeted() const
		{
			return _linkedRepresentation->getFlagDataTargeted();
		};

		bool			   hasToDrawSphere() const { return _linkedRepresentation->hasToDrawSphere(); };
		const SphereData & getSphereData() const { return _sphereData.getValue(); };
		void			   setSphereFixedRadius( const float p_radius );
		void			   setSphereAddRadius( const float p_radius );

		bool				 hasToDrawCylinder() const { return _linkedRepresentation->hasToDrawCylinder(); };
		const CylinderData & getCylinderData() const { return _cylinderData.getValue(); };
		void				 setCylinderRadius( const float p_radius );

		bool			   hasToDrawRibbon() const { return _linkedRepresentation->hasToDrawRibbon(); };
		const RibbonData & getRibbonData() const { return _ribbonData.getValue(); };

		const BaseRepresentation * const getLinkedRepresentation() const { return _linkedRepresentation; }
		BaseRepresentation * const		 getLinkedRepresentation() { return _linkedRepresentation; }

	  protected:
		BaseRepresentation * const	 _linkedRepresentation;
		Generic::BaseRepresentable * _target = nullptr;

		Generic::OverridableParameter<Color::Rgb>		   _color;
		Generic::OverridableParameter<Generic::COLOR_MODE> _colorMode;

		Generic::OverridableParameter<SphereData>	_sphereData;
		Generic::OverridableParameter<CylinderData> _cylinderData;
		Generic::OverridableParameter<RibbonData>	_ribbonData;

		void _updateTargets( const VTX::Representation::MoleculeComputationFlag & p_flag );
	};

} // namespace VTX::Model::Representation
#endif
