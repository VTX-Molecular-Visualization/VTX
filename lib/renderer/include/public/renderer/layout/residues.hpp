#ifndef __VTX_RENDERER_LAYOUT_RESIDUES__
#define __VTX_RENDERER_LAYOUT_RESIDUES__

#include "base_layout.hpp"

namespace VTX::Renderer::Layout
{
	enum struct RESIDUE_ATTR
	{
		POSITION,
		DIRECTION,
		TYPE,
		COLOR,
		ID,
		FLAG,
		REPRESENTATION
	};

	class Residues : public BaseLayout
	{
	  public:
		Residues()
		{
			attributes.push_back( { RESIDUES_POSITIONS, Desc::E_TYPE::VEC4F } );
			attributes.push_back( { RESIDUES_DIRECTIONS, Desc::E_TYPE::VEC3F } );
			attributes.push_back( { RESIDUES_TYPES, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { RESIDUES_COLORS, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { RESIDUES_IDS, Desc::E_TYPE::UINT } );
			attributes.push_back( { RESIDUES_FLAGS, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { RESIDUES_REPRESENTATIONS, Desc::E_TYPE::UBYTE } );
		}

		template<RESIDUE_ATTR A, typename T>
		void upload( Context::ContextWrapper & p_context, const Desc::Handle p_handle, std::span<const T> p_data )
		{
			assert( p_data.size() <= size( p_handle ) );

			const Index o = offset( p_handle );

			if constexpr ( A == RESIDUE_ATTR::POSITION )
			{
				p_context.setBuffer<Vec4f>( { RESIDUES_POSITIONS }, p_data, o );
			}
			else if constexpr ( A == RESIDUE_ATTR::DIRECTION )
			{
				p_context.setBuffer<Vec3f>( { RESIDUES_DIRECTIONS }, p_data, o );
			}
			else if constexpr ( A == RESIDUE_ATTR::TYPE )
			{
				p_context.setBuffer<uint8_t>( { RESIDUES_TYPES }, p_data, o );
			}
			else if constexpr ( A == RESIDUE_ATTR::COLOR )
			{
				p_context.setBuffer<ColorIndex>( { RESIDUES_COLORS }, p_data, o );
			}
			else if constexpr ( A == RESIDUE_ATTR::ID )
			{
				p_context.setBuffer<UID32>( { RESIDUES_IDS }, p_data, o );
			}
			else if constexpr ( A == RESIDUE_ATTR::FLAG )
			{
				p_context.setBuffer<Flag>( { RESIDUES_FLAGS }, p_data, o );
			}
			else if constexpr ( A == RESIDUE_ATTR::REPRESENTATION )
			{
				p_context.setBuffer<RepresentationIndex>( { RESIDUES_REPRESENTATIONS }, p_data, o );
			}
			else
			{
				static_assert( always_false_v<A>, "Invalid residue attribute." );
			}
		}

	  protected:
		void _resize( Context::ContextWrapper & p_context, const Index p_size ) override
		{
			p_context.setBuffer<Vec4f>( { RESIDUES_POSITIONS }, p_size );
			p_context.setBuffer<Vec3f>( { RESIDUES_DIRECTIONS }, p_size );
			p_context.setBuffer<uint8_t>( { RESIDUES_TYPES }, p_size );
			p_context.setBuffer<ColorIndex>( { RESIDUES_COLORS }, p_size );
			p_context.setBuffer<UID32>( { RESIDUES_IDS }, p_size );
			p_context.setBuffer<Flag>( { RESIDUES_FLAGS }, p_size );
			p_context.setBuffer<RepresentationIndex>( { RESIDUES_REPRESENTATIONS }, p_size );
		}

	  private:
		inline static const std::string RESIDUES_POSITIONS		 = "Residues.Positions";
		inline static const std::string RESIDUES_DIRECTIONS		 = "Residues.Directions";
		inline static const std::string RESIDUES_TYPES			 = "Residues.Types";
		inline static const std::string RESIDUES_COLORS			 = "Residues.Colors";
		inline static const std::string RESIDUES_IDS			 = "Residues.Ids";
		inline static const std::string RESIDUES_FLAGS			 = "Residues.Flags";
		inline static const std::string RESIDUES_REPRESENTATIONS = "Residues.Representations";
	};
} // namespace VTX::Renderer::Layout

#endif
