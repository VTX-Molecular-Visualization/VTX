#ifndef __VTX_RENDERER_LAYOUT_ATOMS__
#define __VTX_RENDERER_LAYOUT_ATOMS__

#include "base_layout.hpp"

namespace VTX::Renderer::Layout
{
	class Atoms : public BaseLayout
	{
	  public:
		Atoms()
		{
			attributes.push_back( { "Atoms.Positions", Desc::E_TYPE::VEC3F } );
			attributes.push_back( { "Atoms.Colors", Desc::E_TYPE::UBYTE } );
			attributes.push_back( { "Atoms.Radii", Desc::E_TYPE::FLOAT } );
			attributes.push_back( { "Atoms.Ids", Desc::E_TYPE::UINT } );
			attributes.push_back( { "Atoms.Flags", Desc::E_TYPE::UBYTE } );
			attributes.push_back( { "Atoms.Models", Desc::E_TYPE::USHORT } );
			attributes.push_back( { "Atoms.Representations", Desc::E_TYPE::UBYTE } );
		}
	};
} // namespace VTX::Renderer::Layout

#endif
