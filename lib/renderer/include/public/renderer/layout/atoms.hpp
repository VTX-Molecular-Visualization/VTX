#ifndef __VTX_RENDERER_LAYOUT_ATOMS__
#define __VTX_RENDERER_LAYOUT_ATOMS__

#include "base_layout.hpp"

namespace VTX::Renderer::Layout
{
	enum struct ATOM_ATTR
	{
		POSITION,
		RADIUS, // TODO: remove and use glsl constants.
		ID,		// TODO: remove and use gl_DrawID/gl_VertexID?
		COLOR,
		REPRESENTATION,
		MODEL, // TODO: remove and use gl_DrawID.
		FLAG   // TODO: no more use this for visibility.
	};

	class Atoms : public BaseLayout
	{
	  public:
		Atoms()
		{
			attributes.push_back( { ATOMS_POSITIONS, Desc::E_TYPE::VEC3F } );
			attributes.push_back( { ATOMS_RADII, Desc::E_TYPE::FLOAT } );
			attributes.push_back( { ATOMS_IDS, Desc::E_TYPE::UINT } );
			attributes.push_back( { ATOMS_COLORS, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { ATOMS_REPRESENTATIONS, Desc::E_TYPE::UBYTE } );
			attributes.push_back( { ATOMS_MODELS, Desc::E_TYPE::USHORT } );
			attributes.push_back( { ATOMS_FLAGS, Desc::E_TYPE::UBYTE } );
		}

		template<ATOM_ATTR A, typename T>
		void upload( Context::ContextWrapper & p_context, const SystemUID, std::span<const T> p_data )
		{
			if constexpr ( A == ATOM_ATTR::POSITION )
			{
				p_context.setPipelineBuffer<Vec3f>( ATOMS_POSITIONS, p_data );
			}
			else if constexpr ( A == ATOM_ATTR::RADIUS )
			{
				p_context.setPipelineBuffer<float>( ATOMS_RADII, p_data );
			}
			else if constexpr ( A == ATOM_ATTR::ID )
			{
				p_context.setPipelineBuffer<PickingUID>( ATOMS_IDS, p_data );
			}
			else if constexpr ( A == ATOM_ATTR::COLOR )
			{
				p_context.setPipelineBuffer<ColorIndex>( ATOMS_COLORS, p_data );
			}
			else if constexpr ( A == ATOM_ATTR::REPRESENTATION )
			{
				p_context.setPipelineBuffer<RepresentationIndex>( ATOMS_REPRESENTATIONS, p_data );
			}
			else if constexpr ( A == ATOM_ATTR::MODEL )
			{
				p_context.setPipelineBuffer<ModelIndex>( ATOMS_MODELS, p_data );
			}
			else if constexpr ( A == ATOM_ATTR::FLAG )
			{
				p_context.setPipelineBuffer<Flag>( ATOMS_FLAGS, p_data );
			}
			else
			{
				static_assert( always_false_v<A>, "Invalid atom attribute." );
			}
		}

	  protected:
		void _resize( Context::ContextWrapper & p_context ) override
		{
			p_context.setPipelineBuffer<Vec3f>( ATOMS_POSITIONS, size() );
			p_context.setPipelineBuffer<float>( ATOMS_RADII, size() );
			p_context.setPipelineBuffer<PickingUID>( ATOMS_IDS, size() );
			p_context.setPipelineBuffer<ColorIndex>( ATOMS_COLORS, size() );
			p_context.setPipelineBuffer<RepresentationIndex>( ATOMS_REPRESENTATIONS, size() );
			p_context.setPipelineBuffer<ModelIndex>( ATOMS_MODELS, size() );
			p_context.setPipelineBuffer<Flag>( ATOMS_FLAGS, size() );
		}

	  private:
		inline static const std::string ATOMS_POSITIONS		  = "Atoms.Positions";
		inline static const std::string ATOMS_COLORS		  = "Atoms.Colors";
		inline static const std::string ATOMS_RADII			  = "Atoms.Radii";
		inline static const std::string ATOMS_IDS			  = "Atoms.Ids";
		inline static const std::string ATOMS_FLAGS			  = "Atoms.Flags";
		inline static const std::string ATOMS_MODELS		  = "Atoms.Models";
		inline static const std::string ATOMS_REPRESENTATIONS = "Atoms.Representations";
	};
} // namespace VTX::Renderer::Layout

#endif
