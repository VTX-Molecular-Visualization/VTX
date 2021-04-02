#include "representation_library.hpp"
#include "id.hpp"
#include "mvc/mvc_manager.hpp"

namespace VTX::Model::Representation
{
	BaseRepresentation * const RepresentationLibrary::getRepresentation( const int p_index )
	{
		if ( 0 <= p_index && p_index < _representations.size() )
			return _representations[ p_index ];

		return nullptr;
	};
	const BaseRepresentation * const RepresentationLibrary::getRepresentation( const int p_index ) const
	{
		if ( 0 <= p_index && p_index < _representations.size() )
			return _representations[ p_index ];

		return nullptr;
	};
	BaseRepresentation * const RepresentationLibrary::getRepresentationByName( const std::string & p_name )
	{
		for ( BaseRepresentation * const representation : _representations )
		{
			if ( representation->getName() == p_name )
				return representation;
		}

		return nullptr;
	};
	const BaseRepresentation * const RepresentationLibrary::getRepresentationByName( const std::string & p_name ) const
	{
		for ( const BaseRepresentation * const representation : _representations )
		{
			if ( representation->getName() == p_name )
				return representation;
		}

		return nullptr;
	};

	int RepresentationLibrary::getRepresentationIndex( const BaseRepresentation * const p_representation ) const
	{
		for ( int i = 0; i < _representations.size(); i++ )
		{
			if ( _representations[ i ] == p_representation )
				return i;
		}

		return -1;
	}

	int RepresentationLibrary::getRepresentationCount() const { return int( _representations.size() ); };

	void RepresentationLibrary::addRepresentation( BaseRepresentation * const p_representation )
	{
		_representations.emplace_back( p_representation );
	};
	void RepresentationLibrary::removeRepresentation( const int p_index )
	{
		if ( 0 <= p_index && p_index < _representations.size() )
			_representations.erase( _representations.begin() + p_index );
	};

	void RepresentationLibrary::_init()
	{
		Representation_Sticks * stick = MVC::MvcManager::get().instantiateModel<Representation_Sticks>();
		stick->setName( "Stick" );
		stick->setStickRadius( 0.15f );
		stick->setColorMode( Generic::COLOR_MODE::ATOM_CHAIN );
		stick->setIconPath( ":/sprite/representation_stick_icon.png" );
		_representations.emplace_back( stick );

		Representation_VanDerWaals * vdw = MVC::MvcManager::get().instantiateModel<Representation_VanDerWaals>();
		vdw->setName( "VdW" );
		vdw->setColorMode( Generic::COLOR_MODE::CHAIN );
		vdw->setIconPath( ":/sprite/representation_trace_icon.png" );
		_representations.emplace_back( vdw );

		Representation_Sas * sas = MVC::MvcManager::get().instantiateModel<Representation_Sas>();
		sas->setName( "SAS" );
		sas->setColorMode( Generic::COLOR_MODE::CHAIN );
		sas->setIconPath( ":/sprite/representation_sas_icon.png" );
		_representations.emplace_back( sas );

		Representation_BallsAndSticks * ballsAndsticks
			= MVC::MvcManager::get().instantiateModel<Representation_BallsAndSticks>();
		ballsAndsticks->setName( "Ball And Stick" );
		ballsAndsticks->setColorMode( Generic::COLOR_MODE::CHAIN );
		ballsAndsticks->setIconPath( "" );
		_representations.emplace_back( ballsAndsticks );

		Representation_Cartoon * cartoon = MVC::MvcManager::get().instantiateModel<Representation_Cartoon>();
		cartoon->setName( "Cartoon" );
		cartoon->setColorMode( Generic::COLOR_MODE::CHAIN );
		cartoon->setIconPath( "" );
		_representations.emplace_back( cartoon );

		Representation_StickAndCartoon * stickAndCartoon
			= MVC::MvcManager::get().instantiateModel<Representation_StickAndCartoon>();
		stickAndCartoon->setName( "Stick + Cartoon" );
		stickAndCartoon->setColorMode( Generic::COLOR_MODE::CHAIN );
		stickAndCartoon->setIconPath( "" );
		_representations.emplace_back( stickAndCartoon );
	}
} // namespace VTX::Model::Representation
