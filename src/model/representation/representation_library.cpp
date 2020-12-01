#include "representation_library.hpp"
#include "id.hpp"

namespace VTX
{
	namespace Model
	{
		namespace Representation
		{
			void RepresentationLibrary::init()
			{
				Representation_Sticks * stick = new Representation_Sticks();
				stick->setStickRadius( 0.5f );
				_representations.emplace_back( stick );

				Representation_VanDerWaals * vdw = new Representation_VanDerWaals();
				_representations.emplace_back( vdw );

				Representation_Sas * sas = new Representation_Sas();
				_representations.emplace_back( sas );
			}
		} // namespace Representation
	}	  // namespace Model
} // namespace VTX
