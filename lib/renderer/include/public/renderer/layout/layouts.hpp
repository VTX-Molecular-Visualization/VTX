#ifndef __VTX_RENDERER_LAYOUTS__
#define __VTX_RENDERER_LAYOUTS__

#include "renderer/layout/atoms.hpp"
#include "renderer/layout/residues.hpp"
#include "renderer/layout/voxels.hpp"

namespace VTX::Renderer
{
	struct Layouts
	{
		Layout::Atoms	   atoms;
		Layout::Residues   residues;
		Layout::Voxels	   voxels;
	};
} // namespace VTX::Renderer

#endif
