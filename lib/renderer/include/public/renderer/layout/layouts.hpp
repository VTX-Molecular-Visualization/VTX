#ifndef __VTX_RENDERER_LAYOUTS__
#define __VTX_RENDERER_LAYOUTS__

#include "renderer/layout/atoms.hpp"
#include "renderer/layout/meshes.hpp"
#include "renderer/layout/residues.hpp"
#include "renderer/layout/voxels.hpp"

namespace VTX::Renderer
{
	class Layouts
	{
	  public:
		Layout::Atoms	 atoms;
		Layout::Meshes	 meshes;
		Layout::Residues residues;
		Layout::Voxels	 voxels;

		void clear()
		{
			atoms.clear();
			meshes.clear();
			residues.clear();
			voxels.clear();
		}

		void clearSystems()
		{
			atoms.clear();
			residues.clear();
		}

		void resize( Context::ContextWrapper & p_context )
		{
			atoms.resize( p_context );
			meshes.resize( p_context );
			residues.resize( p_context );
			voxels.resize( p_context );
		}

		void resizeSystems( Context::ContextWrapper & p_context )
		{
			atoms.resize( p_context );
			residues.resize( p_context );
		}
	};
} // namespace VTX::Renderer

#endif
