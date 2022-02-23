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
	Generic::COLOR_MODE &		RepresentationData::getColorMode() { return _colorMode; }
	void						RepresentationData::setColorMode( const Generic::COLOR_MODE & p_colorMode )
	{
		_colorMode = p_colorMode;
		notifyRepresentationDataChange();
	}

	const Generic::SECONDARY_STRUCTURE_COLOR_MODE & RepresentationData::getSecondaryStructureColorMode() const
	{
		return _ssColorMode;
	}
	Generic::SECONDARY_STRUCTURE_COLOR_MODE & RepresentationData::getSecondaryStructureColorMode()
	{
		return _ssColorMode;
	}
	void RepresentationData::setSecondaryStructureColorMode(
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode )
	{
		_ssColorMode = p_colorMode;
		notifyRepresentationDataChange();
	}

	const Generic::TRANSITION_COLOR_MODE & RepresentationData::getTransitionColorMode() const
	{
		return _colorTransitionMode;
	}
	Generic::TRANSITION_COLOR_MODE & RepresentationData::getTransitionColorMode() { return _colorTransitionMode; }
	void RepresentationData::setTransitionColorMode( const Generic::TRANSITION_COLOR_MODE & p_colorTransitionMode )
	{
		_colorTransitionMode = p_colorTransitionMode;
		notifyRepresentationDataChange();
	}

	const Generic::TRANSITION_COLOR_MODE & RepresentationData::getSecondaryStructureTransitionColorMode() const
	{
		return _ssColorTransitionMode;
	}
	Generic::TRANSITION_COLOR_MODE & RepresentationData::getSecondaryStructureTransitionColorMode()
	{
		return _ssColorTransitionMode;
	}
	void RepresentationData::setSecondaryStructureTransitionColorMode(
		const Generic::TRANSITION_COLOR_MODE & p_colorTransitionMode )
	{
		_ssColorTransitionMode = p_colorTransitionMode;
		notifyRepresentationDataChange();
	}

	void RepresentationData::setSphereRadius( float p_radius )
	{
		if ( _sphereData != nullptr )
		{
			_sphereData->_radiusFixed = p_radius;
			notifyRepresentationDataChange();
		}
	}
	void RepresentationData::setCylinderRadius( float p_radius )
	{
		if ( _cylinderData != nullptr )
		{
			_cylinderData->_radius = p_radius;
			notifyRepresentationDataChange();
		}
	}

	void RepresentationData::copyData( const RepresentationData & p_source )
	{
		if ( _sphereData != nullptr && p_source._sphereData != nullptr )
		{
			_sphereData->_radiusAdd	  = p_source._sphereData->_radiusAdd;
			_sphereData->_radiusFixed = p_source._sphereData->_radiusFixed;
		}

		if ( _cylinderData != nullptr && p_source._cylinderData != nullptr )
			_cylinderData->_radius = p_source._cylinderData->_radius;

		_colorMode			   = p_source._colorMode;
		_ssColorMode		   = p_source._ssColorMode;
		_colorTransitionMode   = p_source._colorTransitionMode;
		_ssColorTransitionMode = p_source._ssColorTransitionMode;
	}

	void RepresentationData::notifyRepresentationDataChange() { _linkedRepresentation->forceNotifyDataChanged(); }

} // namespace VTX::Model::Representation
