#ifndef __VTX_RENDERER_LAYOUT_RESIDUES__
#define __VTX_RENDERER_LAYOUT_RESIDUES__

#include "base_layout.hpp"
#include "renderer/caches.hpp"
#include "renderer/color.hpp"
#include "renderer/representation.hpp"
#include <string_view>

namespace VTX::Renderer::Layout
{
	enum struct RESIDUE_ATTR : uint8_t
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
			attributes.push_back( { Desc::Key { RESIDUES_POSITIONS }, Desc::E_TYPE::VEC4F } );
			attributes.push_back( { Desc::Key { RESIDUES_DIRECTIONS }, Desc::E_TYPE::VEC3F } );
			attributes.push_back( { Desc::Key { RESIDUES_TYPES }, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { Desc::Key { RESIDUES_COLORS }, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { Desc::Key { RESIDUES_IDS }, Desc::E_TYPE::UINT } );
			attributes.push_back( { Desc::Key { RESIDUES_FLAGS }, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { Desc::Key { RESIDUES_REPRESENTATIONS }, Desc::E_TYPE::UBYTE } );
		}

		template<RESIDUE_ATTR A, typename T>
		void upload( Context::ContextWrapper & p_context, const Desc::Handle p_handle, std::span<const T> p_data )
		{
			assert( p_data.size() <= size( p_handle ) );

			const Index o = offset( p_handle );

			if constexpr ( A == RESIDUE_ATTR::POSITION )
			{
				p_context.setBuffer<Vec4f>( { Desc::Key { RESIDUES_POSITIONS } }, p_data, o );
			}
			else if constexpr ( A == RESIDUE_ATTR::DIRECTION )
			{
				p_context.setBuffer<Vec3f>( { Desc::Key { RESIDUES_DIRECTIONS } }, p_data, o );
			}
			else if constexpr ( A == RESIDUE_ATTR::TYPE )
			{
				p_context.setBuffer<uint8_t>( { Desc::Key { RESIDUES_TYPES } }, p_data, o );
			}
			else if constexpr ( A == RESIDUE_ATTR::COLOR )
			{
				p_context.setBuffer<ColorIndex>( { Desc::Key { RESIDUES_COLORS } }, p_data, o );
			}
			else if constexpr ( A == RESIDUE_ATTR::ID )
			{
				p_context.setBuffer<UID32>( { Desc::Key { RESIDUES_IDS } }, p_data, o );
			}
			else if constexpr ( A == RESIDUE_ATTR::FLAG )
			{
				p_context.setBuffer<Flag>( { Desc::Key { RESIDUES_FLAGS } }, p_data, o );
			}
			else if constexpr ( A == RESIDUE_ATTR::REPRESENTATION )
			{
				p_context.setBuffer<RepresentationIndex>( { Desc::Key { RESIDUES_REPRESENTATIONS } }, p_data, o );
			}
			else
			{
				static_assert( always_false_v<A>, "Invalid residue attribute." );
			}
		}

	  protected:
		void _resize( Context::ContextWrapper & p_context, const Index p_size ) override
		{
			p_context.setBuffer<Vec4f>( { Desc::Key { RESIDUES_POSITIONS } }, p_size );
			p_context.setBuffer<Vec3f>( { Desc::Key { RESIDUES_DIRECTIONS } }, p_size );
			p_context.setBuffer<uint8_t>( { Desc::Key { RESIDUES_TYPES } }, p_size );
			p_context.setBuffer<ColorIndex>( { Desc::Key { RESIDUES_COLORS } }, p_size );
			p_context.setBuffer<UID32>( { Desc::Key { RESIDUES_IDS } }, p_size );
			p_context.setBuffer<Flag>( { Desc::Key { RESIDUES_FLAGS } }, p_size );
			p_context.setBuffer<RepresentationIndex>( { Desc::Key { RESIDUES_REPRESENTATIONS } }, p_size );
		}

	  private:
		static constexpr std::string_view RESIDUES_POSITIONS	   = "Residues.Positions";
		static constexpr std::string_view RESIDUES_DIRECTIONS	   = "Residues.Directions";
		static constexpr std::string_view RESIDUES_TYPES		   = "Residues.Types";
		static constexpr std::string_view RESIDUES_COLORS		   = "Residues.Colors";
		static constexpr std::string_view RESIDUES_IDS			   = "Residues.Ids";
		static constexpr std::string_view RESIDUES_FLAGS		   = "Residues.Flags";
		static constexpr std::string_view RESIDUES_REPRESENTATIONS = "Residues.Representations";
	};
} // namespace VTX::Renderer::Layout

#endif
