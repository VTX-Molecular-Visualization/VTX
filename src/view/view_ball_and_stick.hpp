#ifndef __VTX_VIEW_3D_BALL_AND_STICK__
#define __VTX_VIEW_3D_BALL_AND_STICK__

#include "base_view_3d_molecule.hpp"

namespace VTX
{
	namespace View
	{
		class ViewBallAndStick : BaseView3DMolecule
		{
		  public:
			ViewBallAndStick() {}
			virtual void update( const uint p_time ) override {}
		};
	} // namespace View
} // namespace VTX
#endif
