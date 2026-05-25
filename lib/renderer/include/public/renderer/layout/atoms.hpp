#ifndef __VTX_RENDERER_LAYOUT_ATOMS__
#define __VTX_RENDERER_LAYOUT_ATOMS__

#include "base_layout.hpp"

namespace VTX::Renderer::Layout
{
	enum struct ATOM_ATTR
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
		inline static const std::string ATOMS_POSITIONS		  = "Atoms.Positions";
		inline static const std::string ATOMS_COLORS		  = "Atoms.Colors";
		inline static const std::string ATOMS_SYMBOLS		  = "Atoms.Symbols";
		inline static const std::string ATOMS_IDS			  = "Atoms.Ids";
		inline static const std::string ATOMS_FLAGS			  = "Atoms.Flags";
		inline static const std::string ATOMS_REPRESENTATIONS = "Atoms.Representations";

		inline static constexpr Desc::Binding BINDING_ATOMS_COLORS = 8;
		inline static constexpr Desc::Binding BINDING_ATOMS_FLAGS  = 9;

		Atoms()
		{
			attributes.push_back( { ATOMS_POSITIONS, Desc::E_TYPE::VEC3F } );
			attributes.push_back( { ATOMS_COLORS, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { ATOMS_SYMBOLS, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { ATOMS_IDS, Desc::E_TYPE::UINT } );
			attributes.push_back( { ATOMS_FLAGS, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { ATOMS_REPRESENTATIONS, Desc::E_TYPE::UBYTE } );
		}

		template<ATOM_ATTR A, typename T>
		void upload( Context::ContextWrapper & p_context, const Desc::Handle p_handle, std::span<const T> p_data )
		{
			assert( p_data.size() <= size( p_handle ) );

			const Index o = offset( p_handle );

			if constexpr ( A == ATOM_ATTR::POSITION )
			{
				p_context.setBuffer<Vec3f>( { ATOMS_POSITIONS }, p_data, o );
			}
			else if constexpr ( A == ATOM_ATTR::SYMBOL )
			{
				p_context.setBuffer<Symbol>( { ATOMS_SYMBOLS }, p_data, o );
			}
			else if constexpr ( A == ATOM_ATTR::ID )
			{
				p_context.setBuffer<UID32>( { ATOMS_IDS }, p_data, o );
			}
			else if constexpr ( A == ATOM_ATTR::COLOR )
			{
				p_context.setBuffer<ColorIndex>( { ATOMS_COLORS }, p_data, o );
			}
			else if constexpr ( A == ATOM_ATTR::REPRESENTATION )
			{
				p_context.setBuffer<RepresentationIndex>( { ATOMS_REPRESENTATIONS }, p_data, o );
			}
			else if constexpr ( A == ATOM_ATTR::FLAG )
			{
				p_context.setBuffer<Flag>( { ATOMS_FLAGS }, p_data, o );
			}
			else
			{
				static_assert( always_false_v<A>, "Invalid atom attribute." );
			}
		}

	  protected:
		void _resize( Context::ContextWrapper & p_context, const Index p_size ) override
		{
			p_context.setBuffer<Vec3f>( { ATOMS_POSITIONS }, p_size );
			p_context.setBuffer<Symbol>( { ATOMS_SYMBOLS }, p_size );
			p_context.setBuffer<UID32>( { ATOMS_IDS }, p_size );
			p_context.setBuffer<uint32_t>( { ATOMS_COLORS }, ( p_size + 3 ) / 4 );
			p_context.setBuffer<RepresentationIndex>( { ATOMS_REPRESENTATIONS }, p_size );
			p_context.setBuffer<uint32_t>( { ATOMS_FLAGS }, ( p_size + 3 ) / 4 );
		}
	};
} // namespace VTX::Renderer::Layout

#endif
