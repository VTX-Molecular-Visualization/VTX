#include "triangle.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			void Triangle::init() {}

			void Triangle::notify( const Event::VTX_EVENT_MODEL & p_event ) { BaseView3D::notify( p_event ); };

			void Triangle::render() {}
		} // namespace D3
	}	  // namespace View
} // namespace VTX
