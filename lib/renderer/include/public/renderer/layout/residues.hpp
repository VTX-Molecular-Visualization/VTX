#ifndef __VTX_RENDERER_LAYOUT_RESIDUES__
#define __VTX_RENDERER_LAYOUT_RESIDUES__

#include "base_layout.hpp"

namespace VTX::Renderer::Layout
{
	class Residues : public BaseLayout
	{
	  public:
		Residues()
		{
			attributes.push_back( { "Residues.Positions", Desc::E_TYPE::VEC4F } );
			attributes.push_back( { "Residues.Directions", Desc::E_TYPE::VEC3F } );
			attributes.push_back( { "Residues.Types", Desc::E_TYPE::UBYTE } );
			attributes.push_back( { "Residues.Colors", Desc::E_TYPE::UBYTE } );
			attributes.push_back( { "Residues.Ids", Desc::E_TYPE::UINT } );
			attributes.push_back( { "Residues.Flags", Desc::E_TYPE::UBYTE } );
			attributes.push_back( { "Residues.Models", Desc::E_TYPE::USHORT } );
			attributes.push_back( { "Residues.Representations", Desc::E_TYPE::UBYTE } );
		}
	};
} // namespace VTX::Renderer::Layout

#endif
