#ifndef __VTX_MODEL_REPRESENTATION_DATA__
#define __VTX_MODEL_REPRESENTATION_DATA__

#include "app/application/representation/_fwd.hpp"
#include "app/application/representation/enum_representation.hpp"
#include "app/application/representation/primitive/cylinder.hpp"
#include "app/application/representation/primitive/ribbon.hpp"
#include "app/application/representation/primitive/ses.hpp"
#include "app/application/representation/primitive/sphere.hpp"
#include "app/internal/chemdb/color.hpp"
#include <util/color/rgba.hpp>

namespace VTX::App::Application::Representation
{
	class RepresentationData
	{
	  public:
		static TARGETED_DATA_FLAG getFlagDataTargeted( const REPRESENTATION_ENUM & p_representation );

	  public:
		RepresentationData( const REPRESENTATION_ENUM & p_representationType );
		virtual ~RepresentationData();

		void setLinkedRepresentation( RepresentationPreset * const p_representation )
		{
			_linkedRepresentation = p_representation;
		}

		const REPRESENTATION_ENUM & getRepresentationType() const;

		const App::Internal::ChemDB::Color::COLOR_MODE & getColorMode() const;
		void setColorMode( const App::Internal::ChemDB::Color::COLOR_MODE & p_colorMode );

		const TARGETED_DATA_FLAG getFlagDataTargeted() const
		{
			RepresentationData::getFlagDataTargeted( _representationType );
		};

		bool					  hasToDrawSphere() const { return _sphereData != nullptr; };
		const Primitive::Sphere & getSphereData() const { return *_sphereData; };
		Primitive::Sphere &		  getSphereData() { return *_sphereData; };
		virtual float			  getSphereRadius() const
		{
			assert( _sphereData != nullptr );
			return _sphereData->radiusFixed;
		};
		virtual void setSphereRadius( float p_radius );

		bool						hasToDrawCylinder() const { return _cylinderData != nullptr; };
		const Primitive::Cylinder & getCylinderData() const { return *_cylinderData; };
		Primitive::Cylinder &		getCylinderData() { return *_cylinderData; };
		virtual float				getCylinderRadius() const
		{
			assert( _cylinderData != nullptr );
			return _cylinderData->radius;
		};
		virtual void											  setCylinderRadius( float p_radius );
		const App::Internal::ChemDB::Color::COLOR_BLENDING_MODE & getCylinderColorBlendingMode() const
		{
			assert( _cylinderData != nullptr );
			return _cylinderData->colorBlendingMode;
		}
		virtual void setCylinderColorBlendingMode( const App::Internal::ChemDB::Color::COLOR_BLENDING_MODE & );

		bool					  hasToDrawRibbon() const { return _ribbonData != nullptr; };
		const Primitive::Ribbon & getRibbonData() const { return *_ribbonData; };
		Primitive::Ribbon &		  getRibbonData() { return *_ribbonData; };
		const App::Internal::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE & getRibbonColorMode() const
		{
			assert( _ribbonData != nullptr );
			return _ribbonData->colorMode;
		}
		virtual void setRibbonColorMode( const App::Internal::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE & );
		const App::Internal::ChemDB::Color::COLOR_BLENDING_MODE & getRibbonColorBlendingMode() const
		{
			assert( _ribbonData != nullptr );
			return _ribbonData->colorBlendingMode;
		}
		virtual void setRibbonColorBlendingMode( const App::Internal::ChemDB::Color::COLOR_BLENDING_MODE & );

		bool				   hasToDrawSES() const { return _sesData != nullptr; };
		const Primitive::SES & getSESData() const { return *_sesData; };
		Primitive::SES &	   getSESData() { return *_sesData; };
		const float			   getSESResolution() const
		{
			assert( _sesData != nullptr );
			return _sesData->resolution;
		}
		virtual void setSESResolution( const float );

		void copyData( const RepresentationData & p_source );

	  protected:
		const REPRESENTATION_ENUM _representationType = REPRESENTATION_ENUM::COUNT;

		Primitive::Sphere *	  _sphereData	= nullptr;
		Primitive::Cylinder * _cylinderData = nullptr;
		Primitive::Ribbon *	  _ribbonData	= nullptr;
		Primitive::SES *	  _sesData		= nullptr;

		App::Internal::ChemDB::Color::COLOR_MODE _colorMode = App::Internal::ChemDB::Color::COLOR_MODE::ATOM_CHAIN;

		void notifyRepresentationDataChange();

	  private:
		RepresentationPreset * _linkedRepresentation = nullptr;
	};
} // namespace VTX::App::Application::Representation
#endif
