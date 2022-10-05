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
		SPHERE_RADIUS_FIXED			 = 1 << 0,
		SPHERE_RADIUS_ADD			 = 1 << 1,
		CYLINDER_RADIUS				 = 1 << 2,
		CYLINDER_COLOR_BLENDING_MODE = 1 << 3,
		RIBBON_COLOR_MODE			 = 1 << 4,
		RIBBON_COLOR_BLENDING_MODE	 = 1 << 5,
		COLOR						 = 1 << 6,
		COLOR_MODE					 = 1 << 7,
		SES_RESOLUTION				 = 1 << 8,

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
		void						setColorMode( const Generic::COLOR_MODE & p_colorMode );

		const VTX::Representation::FlagDataTargeted getFlagDataTargeted() const
		{
			return VTX::Representation::getFlagDataTargeted( _representationType );
		};

		bool			   hasToDrawSphere() const { return _sphereData != nullptr; };
		const SphereData & getSphereData() const { return *_sphereData; };
		SphereData &	   getSphereData() { return *_sphereData; };
		virtual float	   getSphereRadius() const
		{
			assert( _sphereData != nullptr );
			return _sphereData->radiusFixed;
		};
		virtual void setSphereRadius( float p_radius );

		bool				 hasToDrawCylinder() const { return _cylinderData != nullptr; };
		const CylinderData & getCylinderData() const { return *_cylinderData; };
		CylinderData &		 getCylinderData() { return *_cylinderData; };
		virtual float		 getCylinderRadius() const
		{
			assert( _cylinderData != nullptr );
			return _cylinderData->radius;
		};
		virtual void						 setCylinderRadius( float p_radius );
		const Generic::COLOR_BLENDING_MODE & getCylinderColorBlendingMode() const
		{
			assert( _cylinderData != nullptr );
			return _cylinderData->colorBlendingMode;
		}
		virtual void setCylinderColorBlendingMode( const Generic::COLOR_BLENDING_MODE & );

		bool											hasToDrawRibbon() const { return _ribbonData != nullptr; };
		const RibbonData &								getRibbonData() const { return *_ribbonData; };
		RibbonData &									getRibbonData() { return *_ribbonData; };
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE & getRibbonColorMode() const
		{
			assert( _ribbonData != nullptr );
			return _ribbonData->colorMode;
		}
		virtual void						 setRibbonColorMode( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & );
		const Generic::COLOR_BLENDING_MODE & getRibbonColorBlendingMode() const
		{
			assert( _ribbonData != nullptr );
			return _ribbonData->colorBlendingMode;
		}
		virtual void setRibbonColorBlendingMode( const Generic::COLOR_BLENDING_MODE & );

		bool			hasToDrawSES() const { return _sesData != nullptr; };
		const SESData & getSESData() const { return *_sesData; };
		SESData &		getSESData() { return *_sesData; };
		const float		getSESResolution() const
		{
			assert( _sesData != nullptr );
			return _sesData->resolution;
		}
		virtual void setSESResolution( const float );

		void copyData( const RepresentationData & p_source );

	  protected:
		const Generic::REPRESENTATION _representationType = Generic::REPRESENTATION::COUNT;

		SphereData *   _sphereData	 = nullptr;
		CylinderData * _cylinderData = nullptr;
		RibbonData *   _ribbonData	 = nullptr;
		SESData *	   _sesData		 = nullptr;

		Generic::COLOR_MODE _colorMode = Generic::COLOR_MODE::ATOM_CHAIN;

		void notifyRepresentationDataChange();

	  private:
		Representation * _linkedRepresentation = nullptr;
	};
} // namespace VTX::Model::Representation
#endif
