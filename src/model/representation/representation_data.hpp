#ifndef __VTX_MODEL_REPRESENTATION_DATA__
#define __VTX_MODEL_REPRESENTATION_DATA__

#include "color/rgb.hpp"
#include "generic/base_colorable.hpp"
#include "representation_enum.hpp"
#include "representation_primitive.hpp"

namespace VTX::Model::Representation
{
	enum MEMBER_FLAG
	{
		SPHERE_RADIUS_FIXED = 1 << 0,
		SPHERE_RADIUS_ADD	= 1 << 1,
		CYLINDER_RADIUS		= 1 << 2,
		COLOR				= 1 << 3,
		COLOR_MODE			= 1 << 4,
		SS_COLOR_MODE		= 1 << 5,

		NONE = 0,
		ALL	 = 0xFFFF
	};

	class Representation;

	class RepresentationData
	{
	  public:
		RepresentationData( const Generic::REPRESENTATION & p_representationType );
		virtual ~RepresentationData();

		void setLinkedRepresentation( Representation * const p_representation )
		{
			_linkedRepresentation = p_representation;
		}

		const Generic::REPRESENTATION & getRepresentationType() const;

		const Generic::COLOR_MODE & getColorMode() const;
		Generic::COLOR_MODE &		getColorMode();
		void						setColorMode( const Generic::COLOR_MODE & p_colorMode );

		const Generic::SECONDARY_STRUCTURE_COLOR_MODE & getSecondaryStructureColorMode() const;
		Generic::SECONDARY_STRUCTURE_COLOR_MODE &		getSecondaryStructureColorMode();
		void setSecondaryStructureColorMode( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode );

		const VTX::Representation::FlagDataTargeted getFlagDataTargeted() const
		{
			return VTX::Representation::getFlagDataTargeted( _representationType );
		};

		bool			   hasToDrawSphere() const { return _sphereData != nullptr; };
		const SphereData & getSphereData() const { return *_sphereData; };
		SphereData &	   getSphereData() { return *_sphereData; };
		virtual float	   getSphereRadius() const { return _sphereData == nullptr ? 0 : _sphereData->_radiusFixed; };
		virtual void	   setSphereRadius( float p_radius );

		bool				 hasToDrawCylinder() const { return _cylinderData != nullptr; };
		const CylinderData & getCylinderData() const { return *_cylinderData; };
		CylinderData &		 getCylinderData() { return *_cylinderData; };
		virtual float getCylinderRadius() const { return _cylinderData == nullptr ? 0 : _cylinderData->_radius; };
		virtual void  setCylinderRadius( float p_radius );

		bool			   hasToDrawRibbon() const { return _ribbonData != nullptr; };
		const RibbonData & getRibbonData() const { return *_ribbonData; };
		RibbonData &	   getRibbonData() { return *_ribbonData; };

		void copyData( const RepresentationData & p_source );

	  protected:
		const Generic::REPRESENTATION _representationType = Generic::REPRESENTATION::COUNT;

		SphereData *   _sphereData	 = nullptr;
		CylinderData * _cylinderData = nullptr;
		RibbonData *   _ribbonData	 = nullptr;

		Generic::COLOR_MODE						_colorMode	 = Generic::COLOR_MODE::ATOM_CHAIN;
		Generic::SECONDARY_STRUCTURE_COLOR_MODE _ssColorMode = Generic::SECONDARY_STRUCTURE_COLOR_MODE::JMOL;

		void notifyRepresentationDataChange();

	  private:
		Representation * _linkedRepresentation = nullptr;
	};
} // namespace VTX::Model::Representation
#endif
