#ifndef __VTX_MODEL_REPRESENTATION__
#define __VTX_MODEL_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "generic/base_colorable.hpp"
#include "id.hpp"
#include "model/base_model.hpp"
#include "representation_data.hpp"
#include "representation_enum.hpp"
#include "util/math.hpp"

namespace VTX::Model::Representation
{
	class BaseRepresentation : public BaseModel
	{
	  public:
		static const Color::Rgb & generateProteinColor( const int p_seed );

	  public:
		BaseRepresentation( const ID::VTX_ID & p_typeId );
		~BaseRepresentation();

		const Generic::REPRESENTATION & getRepresentationType() const { return _representationType; };

		const std::string & getName() const { return _name; };
		void				setName( const std::string & p_name ) { _name = std::string( p_name ); };

		const std::string & getIconPath() const { return _iconPath; };
		void				setIconPath( const std::string & p_iconPath ) { _iconPath = std::string( p_iconPath ); };

		const Generic::COLOR_MODE & getColorMode() const { return _colorMode; }
		Generic::COLOR_MODE &		getColorMode() { return _colorMode; }
		void setColorMode( const Generic::COLOR_MODE & p_colorMode ) { _colorMode = p_colorMode; }

		const Color::Rgb & getColor() const { return _color; };
		Color::Rgb &	   getColor() { return _color; };
		void			   setColor( const Color::Rgb & p_color ) { _color = Color::Rgb( p_color ); };

		const VTX::Representation::FlagDataTargeted & getFlagDataTargeted() const { return _dataTargeted; };

		bool			   hasToDrawSphere() const { return _sphereData != nullptr; };
		const SphereData & getSphereData() const { return *_sphereData; };

		bool				 hasToDrawCylinder() const { return _cylinderData != nullptr; };
		const CylinderData & getCylinderData() const { return *_cylinderData; };

		bool			   hasToDrawRibbon() const { return _ribbonData != nullptr; };
		const RibbonData & getRibbonData() const { return *_ribbonData; };

	  protected:
		Generic::REPRESENTATION _representationType = Generic::REPRESENTATION::VAN_DER_WAALS;

		std::string							  _name;
		std::string							  _iconPath;
		Color::Rgb							  _color;
		VTX::Representation::FlagDataTargeted _dataTargeted = VTX::Representation::FlagDataTargeted::NONE;
		Generic::COLOR_MODE					  _colorMode	= Generic::COLOR_MODE::PROTEIN;

		SphereData *   _sphereData	 = nullptr;
		CylinderData * _cylinderData = nullptr;
		RibbonData *   _ribbonData	 = nullptr;
	};

	class Representation_BallsAndSticks : public BaseRepresentation
	{
	  public:
		Representation_BallsAndSticks();

		inline void setBallRadius( const float p_radius )
		{
			_sphereData->_isRadiusFixed = true;
			_sphereData->_radiusFixed	= p_radius;
		};

		float		getStickRadius() { return _cylinderData->_radius; };
		inline void setStickRadius( const float p_radius ) { _cylinderData->_radius = p_radius; };
	};
	class Representation_VanDerWaals : public BaseRepresentation
	{
	  public:
		Representation_VanDerWaals();
	};
	class Representation_Sticks : public BaseRepresentation
	{
	  public:
		Representation_Sticks();

		inline void setStickRadius( const float p_radius )
		{
			_sphereData->_radiusFixed = p_radius;
			_cylinderData->_radius	  = p_radius;
		};
	};
	class Representation_Trace : public BaseRepresentation
	{
	  public:
		Representation_Trace();

		float		getRadius() { return _radius; };
		inline void setRadius( const float p_radius ) { _radius = p_radius; };

	  private:
		float _radius = 0.5f;
	};
	class Representation_Sas : public BaseRepresentation
	{
	  public:
		Representation_Sas();
	};

	class Representation_Cartoon : public BaseRepresentation
	{
	  public:
		Representation_Cartoon();
	};

	class Representation_BallAndStickAndCartoon : public BaseRepresentation
	{
	  public:
		Representation_BallAndStickAndCartoon();
	};

} // namespace VTX::Model::Representation
#endif
