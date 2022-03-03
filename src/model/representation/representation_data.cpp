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

		_sphereData->_radiusFixed = p_radius;
		notifyRepresentationDataChange();
	}
	void RepresentationData::setCylinderRadius( float p_radius )
	{
		assert( _cylinderData != nullptr );

		_cylinderData->_radius = p_radius;
		notifyRepresentationDataChange();
	}

	void RepresentationData::setCylinderColorBlendingMode( const Generic::COLOR_BLENDING_MODE & p_colorBlendingMode )
	{
		assert( _cylinderData != nullptr );

		_cylinderData->_colorBlendingMode = p_colorBlendingMode;
		notifyRepresentationDataChange();
	}

	void RepresentationData::setRibbonColorMode( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode )
	{
		assert( _ribbonData != nullptr );

		_ribbonData->_colorMode = p_colorMode;
		notifyRepresentationDataChange();
	}

	void RepresentationData::setRibbonColorBlendingMode( const Generic::COLOR_BLENDING_MODE & p_colorBlendingMode )
	{
		assert( _ribbonData != nullptr );

		_ribbonData->_colorBlendingMode = p_colorBlendingMode;
		notifyRepresentationDataChange();
	}

	void RepresentationData::copyData( const RepresentationData & p_source )
	{
		if ( _sphereData != nullptr && p_source._sphereData != nullptr )
		{
			_sphereData->_radiusAdd	  = p_source._sphereData->_radiusAdd;
			_sphereData->_radiusFixed = p_source._sphereData->_radiusFixed;
		}

		if ( _cylinderData != nullptr && p_source._cylinderData != nullptr )
		{
			_cylinderData->_radius			  = p_source._cylinderData->_radius;
			_cylinderData->_colorBlendingMode = p_source._cylinderData->_colorBlendingMode;
		}

		if ( _ribbonData != nullptr && p_source._ribbonData != nullptr )
		{
			_ribbonData->_colorMode			= p_source._ribbonData->_colorMode;
			_ribbonData->_colorBlendingMode = p_source._ribbonData->_colorBlendingMode;
		}

		_colorMode = p_source._colorMode;
	}

	void RepresentationData::notifyRepresentationDataChange() { _linkedRepresentation->forceNotifyDataChanged(); }

} // namespace VTX::Model::Representation
