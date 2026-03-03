#ifndef __VTX_RENDERER_LAYOUT_VOXELS__
#define __VTX_RENDERER_LAYOUT_VOXELS__

#include "base_layout.hpp"

namespace VTX::Renderer::Layout
{
	class Voxels : public BaseLayout
	{
	  public:
		Voxels()
		{
			attributes.push_back( { "Voxels.Mins", Desc::E_TYPE::VEC3F } );
			attributes.push_back( { "Voxels.Maxs", Desc::E_TYPE::VEC3F } );
		}
	};
} // namespace VTX::Renderer::Layout

#endif
