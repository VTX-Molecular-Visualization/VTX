#include "representation_library.hpp"
#include "id.hpp"
#include "mvc/mvc_manager.hpp"

namespace VTX
{
	namespace Model
	{
		namespace Representation
		{
			void RepresentationLibrary::init()
			{
				Representation_Sticks * stick = MVC::MvcManager::get().instantiateModel<Representation_Sticks>();
				stick->setName( "stick" );
				stick->setStickRadius( 0.15f );
				stick->setColorMode( Generic::COLOR_MODE::CHAIN );
				stick->setIconPath( ":/sprite/representation_stick_icon.png" );
				_representations.emplace_back( stick );

				Representation_VanDerWaals * vdw = MVC::MvcManager::get().instantiateModel<Representation_VanDerWaals>();
				vdw->setName( "vdw" );
				vdw->setColorMode( Generic::COLOR_MODE::CHAIN );
				vdw->setIconPath( ":/sprite/representation_trace_icon.png" );
				_representations.emplace_back( vdw );

				Representation_Sas * sas = MVC::MvcManager::get().instantiateModel<Representation_Sas>();
				sas->setName( "sas" );
				sas->setColorMode( Generic::COLOR_MODE::CHAIN );
				sas->setIconPath( ":/sprite/representation_sas_icon.png" );
				_representations.emplace_back( sas );

				Representation_BallsAndSticks * ballsAndsticks = MVC::MvcManager::get().instantiateModel<Representation_BallsAndSticks>();
				ballsAndsticks->setName( "ballsAndsticks" );
				ballsAndsticks->setColorMode( Generic::COLOR_MODE::CHAIN );
				ballsAndsticks->setIconPath( "" );
				_representations.emplace_back( ballsAndsticks );

				Representation_Sticks * stick2 = MVC::MvcManager::get().instantiateModel<Representation_Sticks>();
				stick2->setName( "large stick" );
				stick2->setStickRadius( 0.5f );
				stick2->setColorMode( Generic::COLOR_MODE::CHAIN );
				stick2->setIconPath( "" );
				_representations.emplace_back( stick2 );

				Representation_Cartoon * cartoon = MVC::MvcManager::get().instantiateModel<Representation_Cartoon>();
				cartoon->setName( "cartoon" );
				cartoon->setColorMode( Generic::COLOR_MODE::CHAIN );
				cartoon->setIconPath( "" );
				_representations.emplace_back( cartoon );
			}
		} // namespace Representation
	}	  // namespace Model
} // namespace VTX