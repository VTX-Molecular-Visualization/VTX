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
				_representations.emplace_back( stick );

				Representation_VanDerWaals * vdw = MVC::MvcManager::get().instantiateModel<Representation_VanDerWaals>();
				vdw->setName( "vdw" );
				_representations.emplace_back( vdw );

				Representation_Sas * sas = MVC::MvcManager::get().instantiateModel<Representation_Sas>();
				sas->setName( "sas" );
				_representations.emplace_back( sas );

				Representation_BallsAndSticks * ballsAndsticks = MVC::MvcManager::get().instantiateModel<Representation_BallsAndSticks>();
				ballsAndsticks->setName( "ballsAndsticks" );
				_representations.emplace_back( ballsAndsticks );

				Representation_Sticks * stick2 = MVC::MvcManager::get().instantiateModel<Representation_Sticks>();
				stick2->setName( "stick2" );
				stick2->setStickRadius( 0.5f );
				_representations.emplace_back( stick2 );

				Representation_Cartoon * cartoon = MVC::MvcManager::get().instantiateModel<Representation_Cartoon>();
				cartoon->setName( "cartoon" );
				_representations.emplace_back( cartoon );
			}
		} // namespace Representation
	}	  // namespace Model
} // namespace VTX
