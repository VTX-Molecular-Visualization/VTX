#ifndef __VTX_VIEW_3D_BALL_AND_STICK__
#define __VTX_VIEW_3D_BALL_AND_STICK__

#include "base_view_3d_molecule.hpp"

namespace VTX
{
	namespace View
	{
		class ViewBallAndStick : public BaseView3DMolecule
		{
		  public:
			ViewBallAndStick() {}
			virtual void render( const uint p_time ) override {};
			virtual void notify( Event::EVENT_MODEL ) {};
		};
	} // namespace View
} // namespace VTX
#endif
