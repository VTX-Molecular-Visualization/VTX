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

namespace VTX
{
	namespace Model
	{
		namespace Representation
		{
			class InstantiatedRepresentation : public BaseModel, public Generic::BaseObjectOverride
			{
			  public:
				InstantiatedRepresentation( BaseRepresentation * const p_linkedRepresentation ) :
					BaseModel( ID::Model::MODEL_INTANTIATED_REPRESENTATION ), _linkedRepresentation( p_linkedRepresentation )
				{
					setName( _linkedRepresentation->getName() + " (instantiated)" );
					_color	   = &_linkedRepresentation->getColor();
					_colorMode = &_linkedRepresentation->getColorMode();

					_priority = getId();
				};

				~InstantiatedRepresentation() {}

				const std::string & getName() const { return _name; };
				void				setName( const std::string & p_name ) { _name = std::string( p_name ); };

				int	 getPriority() const { return _priority; };
				void setPriority( const int p_priority );

				const Color::Rgb &			getColor() const { return *_color; }
				void						setColor( const Color::Rgb & p_color );
				const Generic::COLOR_MODE & getColorMode() const { return *_colorMode; }
				void						setColorMode( const Generic::COLOR_MODE & p_colorMode );

				const VTX::Representation::FlagDataTargeted & getFlagDataTargeted() const { return _linkedRepresentation->getFlagDataTargeted(); };

				bool			   hasToDrawSphere() const { return _linkedRepresentation->hasToDrawSphere(); };
				const SphereData & getSphereData() const { return _linkedRepresentation->getSphereData(); };

				bool				 hasToDrawCylinder() const { return _linkedRepresentation->hasToDrawCylinder(); };
				const CylinderData & getCylinderData() const { return _linkedRepresentation->getCylinderData(); };

				bool			   hasToDrawRibbon() const { return _linkedRepresentation->hasToDrawRibbon(); };
				const RibbonData & getRibbonData() const { return _linkedRepresentation->getRibbonData(); };

			  protected:
				BaseRepresentation * const _linkedRepresentation;
				int						   _priority = 0;

				std::string			  _name;
				Color::Rgb *		  _color;
				Generic::COLOR_MODE * _colorMode;

				SphereData *   _sphereData	 = nullptr;
				CylinderData * _cylinderData = nullptr;
				RibbonData *   _ribbonData	 = nullptr;

				void _updateTargets( const VTX::Representation::MoleculeComputationFlag & p_flag ) const;
			};

		} // namespace Representation
	}	  // namespace Model
} // namespace VTX
#endif
