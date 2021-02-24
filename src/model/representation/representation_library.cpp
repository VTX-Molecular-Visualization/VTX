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
				stick->setName( "Stick" );
				stick->setStickRadius( 0.15f );
				stick->setColorMode( Generic::COLOR_MODE::ATOM_CHAIN );
				stick->setIconPath( ":/sprite/representation_stick_icon.png" );
				_representations.emplace_back( stick );

				Representation_VanDerWaals * vdw
					= MVC::MvcManager::get().instantiateModel<Representation_VanDerWaals>();
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
		} // namespace Representation
	}	  // namespace Model
} // namespace VTX
