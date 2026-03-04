#ifndef __VTX_RENDERER_LAYOUT_VOXELS__
#define __VTX_RENDERER_LAYOUT_VOXELS__

#include "base_layout.hpp"

namespace VTX::Renderer::Layout
{
	enum struct VOXEL_ATTR
	{
		MINS,
		MAXS
	};

	class Voxels : public BaseLayout
	{
	  public:
		Voxels()
		{
			attributes.push_back( { VOXELS_MINS, Desc::E_TYPE::VEC3F } );
			attributes.push_back( { VOXELS_MAXS, Desc::E_TYPE::VEC3F } );
		}

		template<VOXEL_ATTR A, typename T>
		void upload( Context::ContextWrapper & p_context, const SystemUID, std::span<const T> p_data )
		{
			if constexpr ( A == VOXEL_ATTR::MINS )
			{
				p_context.setPipelineBuffer<Vec3f>( VOXELS_MINS, p_data );
			}
			else if constexpr ( A == VOXEL_ATTR::MAXS )
			{
				p_context.setPipelineBuffer<Vec3f>( VOXELS_MAXS, p_data );
			}
			else
			{
				static_assert( always_false_v<A>, "Invalid voxel attribute." );
			}
		}

	  protected:
		void _resize( Context::ContextWrapper & p_context ) override
		{
			p_context.setPipelineBuffer<Vec3f>( VOXELS_MINS, size() );
			p_context.setPipelineBuffer<Vec3f>( VOXELS_MAXS, size() );
		}

	  private:
		inline static const std::string VOXELS_MINS = "Voxels.Mins";
		inline static const std::string VOXELS_MAXS = "Voxels.Maxs";
	};
} // namespace VTX::Renderer::Layout

#endif
