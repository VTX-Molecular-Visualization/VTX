#include "app/application/representation/representation_data.hpp"
#include "app/application/representation/representation_preset.hpp"
#include <util/logger.hpp>

namespace VTX::App::Application::Representation
{
	TARGETED_DATA_FLAG RepresentationData::getFlagDataTargeted( const REPRESENTATION_ENUM & p_representation )
	{
		TARGETED_DATA_FLAG res = TARGETED_DATA_FLAG::ENUM::NONE;

		switch ( p_representation )
		{
		case REPRESENTATION_ENUM::VAN_DER_WAALS:
		case REPRESENTATION_ENUM::SAS: res = TARGETED_DATA_FLAG( TARGETED_DATA_FLAG::ENUM::ATOM ); break;

		case REPRESENTATION_ENUM::CARTOON: res = TARGETED_DATA_FLAG( TARGETED_DATA_FLAG::ENUM::RIBBON ); break;

		case REPRESENTATION_ENUM::BALL_AND_STICK:
		case REPRESENTATION_ENUM::STICK:
			// !V0.1
			// case REPRESENTATION_ENUM::TRACE:
			res = TARGETED_DATA_FLAG( TARGETED_DATA_FLAG::ENUM::ATOM | TARGETED_DATA_FLAG::ENUM::BOND );
			break;

		case REPRESENTATION_ENUM::STICK_AND_CARTOON:
		case REPRESENTATION_ENUM::BALL_AND_STICK_AND_CARTOON:
			res = TARGETED_DATA_FLAG( TARGETED_DATA_FLAG::ENUM::ATOM | TARGETED_DATA_FLAG::ENUM::BOND
									  | TARGETED_DATA_FLAG::ENUM::RIBBON );
			break;

		case REPRESENTATION_ENUM::SES: res = TARGETED_DATA_FLAG( TARGETED_DATA_FLAG::ENUM::SES ); break;

		default:
			VTX_WARNING( "Representation " + std::to_string( int( p_representation ) )
						 + " not managed in Representation::getFlagDataTargeted" );
			break;
		}

		return res;
	}

	RepresentationData::RepresentationData( const REPRESENTATION_ENUM & p_representationType ) :
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

	const REPRESENTATION_ENUM & RepresentationData::getRepresentationType() const { return _representationType; }

	const App::Internal::ChemDB::Color::COLOR_MODE & RepresentationData::getColorMode() const { return _colorMode; }

	void RepresentationData::setColorMode( const App::Internal::ChemDB::Color::COLOR_MODE & p_colorMode )
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

	void RepresentationData::setCylinderColorBlendingMode( const App::Internal::ChemDB::Color::COLOR_BLENDING_MODE & p_colorBlendingMode )
	{
		assert( _cylinderData != nullptr );

		_cylinderData->colorBlendingMode = p_colorBlendingMode;
		notifyRepresentationDataChange();
	}

	void RepresentationData::setRibbonColorMode( const App::Internal::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode )
	{
		assert( _ribbonData != nullptr );

		_ribbonData->colorMode = p_colorMode;
		notifyRepresentationDataChange();
	}

	void RepresentationData::setRibbonColorBlendingMode( const App::Internal::ChemDB::Color::COLOR_BLENDING_MODE & p_colorBlendingMode )
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

} // namespace VTX::App::Application::Representation
