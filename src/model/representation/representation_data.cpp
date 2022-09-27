#include "representation_data.hpp"
#include "representation.hpp"

namespace VTX::Model::Representation
{
	RepresentationData::RepresentationData( const Generic::REPRESENTATION & p_representationType ) :
		_representationType( p_representationType )
	{
	}
	RepresentationData::~RepresentationData()
	{
		if ( _sphereData != nullptr )
			delete _sphereData;
		_sphereData = nullptr;

		if ( _cylinderData != nullptr )
			delete _cylinderData;
		_cylinderData = nullptr;

		if ( _ribbonData != nullptr )
			delete _ribbonData;
		_ribbonData = nullptr;

		if ( _sesData != nullptr )
			delete _sesData;
		_sesData = nullptr;
	}

	const Generic::REPRESENTATION & RepresentationData::getRepresentationType() const { return _representationType; }

	const Generic::COLOR_MODE & RepresentationData::getColorMode() const { return _colorMode; }

	void RepresentationData::setColorMode( const Generic::COLOR_MODE & p_colorMode )
	{
		_colorMode = p_colorMode;
		notifyRepresentationDataChange();
	}

	void RepresentationData::setSphereRadius( float p_radius )
	{
		assert( _sphereData != nullptr );

		_sphereData->radiusFixed = p_radius;
		notifyRepresentationDataChange();
	}
	void RepresentationData::setCylinderRadius( float p_radius )
	{
		assert( _cylinderData != nullptr );

		_cylinderData->radius = p_radius;
		notifyRepresentationDataChange();
	}

	void RepresentationData::setCylinderColorBlendingMode( const Generic::COLOR_BLENDING_MODE & p_colorBlendingMode )
	{
		assert( _cylinderData != nullptr );

		_cylinderData->colorBlendingMode = p_colorBlendingMode;
		notifyRepresentationDataChange();
	}

	void RepresentationData::setRibbonColorMode( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode )
	{
		assert( _ribbonData != nullptr );

		_ribbonData->colorMode = p_colorMode;
		notifyRepresentationDataChange();
	}

	void RepresentationData::setRibbonColorBlendingMode( const Generic::COLOR_BLENDING_MODE & p_colorBlendingMode )
	{
		assert( _ribbonData != nullptr );

		_ribbonData->colorBlendingMode = p_colorBlendingMode;
		notifyRepresentationDataChange();
	}

	void RepresentationData::setSESResolution( const float p_resolution )
	{
		assert( _sesData != nullptr );

		_sesData->resolution = p_resolution;
		notifyRepresentationDataChange();
	}

	void RepresentationData::copyData( const RepresentationData & p_source )
	{
		if ( _sphereData != nullptr && p_source._sphereData != nullptr )
		{
			_sphereData->radiusAdd	 = p_source._sphereData->radiusAdd;
			_sphereData->radiusFixed = p_source._sphereData->radiusFixed;
		}

		if ( _cylinderData != nullptr && p_source._cylinderData != nullptr )
		{
			_cylinderData->radius			 = p_source._cylinderData->radius;
			_cylinderData->colorBlendingMode = p_source._cylinderData->colorBlendingMode;
		}

		if ( _ribbonData != nullptr && p_source._ribbonData != nullptr )
		{
			_ribbonData->colorMode		   = p_source._ribbonData->colorMode;
			_ribbonData->colorBlendingMode = p_source._ribbonData->colorBlendingMode;
		}

		if ( _sesData != nullptr && p_source._sesData != nullptr )
		{
			_sesData->resolution = p_source._sesData->resolution;
		}

		_colorMode = p_source._colorMode;
	}

	void RepresentationData::notifyRepresentationDataChange() { _linkedRepresentation->forceNotifyDataChanged(); }

} // namespace VTX::Model::Representation
