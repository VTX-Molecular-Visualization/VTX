#ifndef __VTX_RENDERER_LAYOUT_ATOMS__
#define __VTX_RENDERER_LAYOUT_ATOMS__

#include "base_layout.hpp"
#include "renderer/caches.hpp"
#include "renderer/color.hpp"
#include "renderer/representation.hpp"
#include <string_view>

namespace VTX::Renderer::Layout
{
	enum struct ATOM_ATTR : uint8_t
	{
		POSITION,
		SYMBOL,
		ID, // TODO: remove and use gl_DrawID/gl_VertexID?
		COLOR,
		REPRESENTATION,
		FLAG // TODO: no more use this for visibility.
	};

	class Atoms : public BaseLayout
	{
	  public:
		static constexpr std::string_view ATOMS_POSITIONS		= "Atoms.Positions";
		static constexpr std::string_view ATOMS_COLORS			= "Atoms.Colors";
		static constexpr std::string_view ATOMS_SYMBOLS			= "Atoms.Symbols";
		static constexpr std::string_view ATOMS_IDS				= "Atoms.Ids";
		static constexpr std::string_view ATOMS_FLAGS			= "Atoms.Flags";
		static constexpr std::string_view ATOMS_REPRESENTATIONS = "Atoms.Representations";

		static constexpr Desc::Binding BINDING_ATOMS_COLORS = 8;
		static constexpr Desc::Binding BINDING_ATOMS_FLAGS	= 9;

		Atoms()
		{
			attributes.push_back( { Desc::Key { ATOMS_POSITIONS }, Desc::E_TYPE::VEC3F } );
			attributes.push_back( { Desc::Key { ATOMS_COLORS }, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { Desc::Key { ATOMS_SYMBOLS }, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { Desc::Key { ATOMS_IDS }, Desc::E_TYPE::UINT } );
			attributes.push_back( { Desc::Key { ATOMS_FLAGS }, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { Desc::Key { ATOMS_REPRESENTATIONS }, Desc::E_TYPE::UBYTE } );
		}

		template<ATOM_ATTR A, typename T>
		void upload( Context::ContextWrapper & p_context, const Desc::Handle p_handle, std::span<const T> p_data )
		{
			assert( p_data.size() <= size( p_handle ) );

			const Index o = offset( p_handle );

			if constexpr ( A == ATOM_ATTR::POSITION )
			{
				p_context.setBuffer<Vec3f>( { Desc::Key { ATOMS_POSITIONS } }, p_data, o );
			}
			else if constexpr ( A == ATOM_ATTR::SYMBOL )
			{
				p_context.setBuffer<Symbol>( { Desc::Key { ATOMS_SYMBOLS } }, p_data, o );
			}
			else if constexpr ( A == ATOM_ATTR::ID )
			{
				p_context.setBuffer<UID32>( { Desc::Key { ATOMS_IDS } }, p_data, o );
			}
			else if constexpr ( A == ATOM_ATTR::COLOR )
			{
				p_context.setBuffer<ColorIndex>( { Desc::Key { ATOMS_COLORS } }, p_data, o );
			}
			else if constexpr ( A == ATOM_ATTR::REPRESENTATION )
			{
				p_context.setBuffer<RepresentationIndex>( { Desc::Key { ATOMS_REPRESENTATIONS } }, p_data, o );
			}
			else if constexpr ( A == ATOM_ATTR::FLAG )
			{
				p_context.setBuffer<Flag>( { Desc::Key { ATOMS_FLAGS } }, p_data, o );
			}
			else
			{
				static_assert( always_false_v<A>, "Invalid atom attribute." );
			}
		}

	  protected:
		void _resize( Context::ContextWrapper & p_context, const Index p_size ) override
		{
			p_context.setBuffer<Vec3f>( { Desc::Key { ATOMS_POSITIONS } }, p_size );
			p_context.setBuffer<Symbol>( { Desc::Key { ATOMS_SYMBOLS } }, p_size );
			p_context.setBuffer<UID32>( { Desc::Key { ATOMS_IDS } }, p_size );
			p_context.setBuffer<uint32_t>( { Desc::Key { ATOMS_COLORS } }, ( p_size + 3 ) / 4 );
			p_context.setBuffer<RepresentationIndex>( { Desc::Key { ATOMS_REPRESENTATIONS } }, p_size );
			p_context.setBuffer<uint32_t>( { Desc::Key { ATOMS_FLAGS } }, ( p_size + 3 ) / 4 );
		}
	};
} // namespace VTX::Renderer::Layout

#endif
