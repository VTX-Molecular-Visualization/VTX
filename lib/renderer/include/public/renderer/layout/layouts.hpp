#ifndef __VTX_RENDERER_LAYOUTS__
#define __VTX_RENDERER_LAYOUTS__

#include "renderer/layout/atoms.hpp"
#include "renderer/layout/residues.hpp"
#include "renderer/layout/voxels.hpp"

namespace VTX::Renderer
{
	class Layouts
	{
	  public:
		Layout::Atoms	 atoms;
		Layout::Residues residues;
		Layout::Voxels	 voxels;

		void resize( Context::ContextWrapper & p_context )
		{
			atoms.resize( p_context );
			residues.resize( p_context );
			voxels.resize( p_context );
		}
	};
} // namespace VTX::Renderer

#endif
