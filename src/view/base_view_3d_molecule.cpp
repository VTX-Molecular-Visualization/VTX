#include "base_view_3d_molecule.hpp"

namespace VTX
{
	namespace View
	{
		void BaseView3DMolecule::notify( Event::EVENT_MODEL p_event )
		{
			if ( p_event == Event::EVENT_MODEL::RENDER ) { draw(); }
		}
	} // namespace View
} // namespace VTX
